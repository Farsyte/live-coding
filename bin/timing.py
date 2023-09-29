#!/usr/bin/python3
#
# prerequisites:
#   pip install 'schemdraw[matplotlib]'

import schemdraw
from schemdraw import elements as elm
from schemdraw import logic


def draw_from_json_string(json, fbase):
    """Convert a WaveDrom JSON string into a PNG file.

    The first parameter is a string containing WaveDrom JSON
    that contains data about how several signals change in
    relation to a clock signal (which may not be present).

    The second parameter is the BASENAME of the file to write,
    which will automatically be placed in doc/img and given
    the .png extension.
    """
    fname = f'doc/img/{fbase}.png'
    with schemdraw.Drawing() as d:
        d += logic.TimingDiagram.from_json(json, risetime = 0, grid=False)
    d.save(fname)
    # print(f'Figure plotted into {fname}')
    return fname

def tosigname(s):
    ob, cb = '{', '}'

    na = s.split(':');
    s = na[-1];

    if s[-1] == '_':
        s = f'/{s[:-1]}'
    fub = s.find('_')
    if fub>0 and fub<len(s)-1:
        s = f'{s[0:fub]}_{ob}{s[fub+1:]}{cb}'
    if s[0] == '/':
        s = f'\\overline{ob}{s[1:]}{cb}'
    na[-1] = s;
    s = ":".join(na);

    return f'${s}$'

def tosigfile(s, session):
    na = s.split(':');
    s = na[-1];
    if s[-1] == '_':
        s = f'{s[:-1]}'
    if s[0] == '/':
        s = f'{s[1:]}'
    if s[0] == '\\':
        s = f'{s[1:]}'
    na[-1] = s;
    s = ":".join(na);
    return f'log/timing/{session}/{s}.log'

def parse_from_trace(sfn, tau_min, tau_len):
    """Construct wave data for a TAU range from a signal trace file.

    Signal data is in f'log/timing/{signal}.log' which contains the
    wave data for WaveDrom. Each line optionally starts with a TAU
    value followed by a colon. Following that are wave data items for
    sequential cycles starting at that TAU. Most wave data are simple
    single characters, but any of "=23456789" take the remainder of
    the line as the textual content of the signal value. See WaveDrom
    documents for the background colors (or try it).

    Leading and trailing whitespace are discarded, as are blank lines.

    Self-contained states are:
    - p, P, n, N: positive and negative clock cycles
    - h, l, H, L: clock high or low for the whole cycle
    - x: value of the signal is unknown
    - z: value of the signal is tri-state
    - 0: the state is a logical zero (low).
    - 1: the state is a logical one (high).

    The state "." is simply "state from prior cycle continues" and is used
    when the next change is in a small TAU. If a trace starts with "." then
    the value "x" is presumed for the prior state.
    """
    tau_lim = tau_min + tau_len
    has_data = {'=','2','3','4','5','6','7','8','9'}
    wa = ['.' for tau in range(tau_len)]
    data = []
    wa[0] = 'x'
    prior_state = None
    prior_data = None
    boring = True
    try:
        with open(sfn) as sf:
            lines = [s.strip() for s in sf.readlines()]
            for line in lines:
                if len(line) > 0:
                    sep = line.find(':')
                    if sep > 0:
                        tau = int(line[0:sep])
                        line = line[sep+1:].strip()
                    for i in range(len(line)):
                        ch = line[i]

                        if tau < tau_min:
                            if ch != '.':
                                prior_state = ch
                                if ch in has_data:
                                    prior_data = line[i+1:]
                                else:
                                    prior_data = None

                        elif tau < tau_lim:
                            if ch != '.':
                                wa[tau - tau_min] = ch
                            if ch in has_data:
                                data.append(line[i+1:])

                        tau += 1
                        if ch in has_data:
                            break

    except FileNotFoundError as e:
        wa = ['.' for tau in range(tau_len)]
        wa[0] = '9'
        data = [f"MISSING FILE: {e.filename}"]

    if prior_state is not None:
        if wa[0] == 'x':
            wa[0] = prior_state
            if prior_data is not None:
                data.insert(0, prior_data)

    if wa[0] == 'x':
        if wa[1] == '.':
            wa[0] = 'z'
            wa[1] = 'x'
            
    return "".join(wa), data

def boring(wave):
    """return true if the wave is boring.
    """
    boring_states = { '.', 'z', 'x' }
    for ch in wave[1:]:
        if ch not in boring_states:
            return False
    return True

def draw_from_json_file(fbase):
    """Update a timing diagram.

    Given the basename of a timing diagram, parse the timing
    data file for the diagram and write the image file.

    The image is written to f'doc/img/{fbase}.png'

    The input file in f'log/timing/{fbase}.txt' defines the layout of
    a timing diagram as follows:

    Leading and trailing whitespace are discarded, as are blank lines.

    The first few lines are as follows:

    - Title, placed at the top of the plot.
    - Caption, placed at the bottom of the plot
    - session name, used to find the signal trace files
    - starting TAU and plot length in TAU units

    Following those lines, each line has a signal name, which will
    be used to find the trace data file, and which will be adjusted
    to form the label for one row of the trace. Leading slash and trailing
    underbar are turned into an "overline"; any other underbar makes the
    rest of the name a subscript.

    A blank row in the diagram can be generated by including an empty
    line in the signal list.

    If title and/or caption contain no text, then their rows will not
    be included in the plot.


    """
    json_sigs = []

    nom_width = 18;

    fname = f'log/timing/{fbase}.txt'

    with open(fname, "r") as pf:

        ob, cb = '{', '}'

        title = pf.readline().strip()
        caption = pf.readline().strip()
        session = pf.readline().strip()
        tau_range =  pf.readline().strip()
        if (tau_range == ''):
            print(f'{fname} has {tau_range=!r}, skipping.')
            return

        tau_min, tau_len = [int(x) for x in tau_range.split(' ')]

        act_len = max(tau_len, nom_width)

        # TITLE line: an unnamed signal with the name as its value.
        # TODO is there a better way to do a title?
        wa = ['.' for tau in range(act_len)]
        wa[0] = '6'
        wave = "".join(wa)
        json_sigs.append(f'    {ob} name: "", wave: "{wave}",\n      data: ["{title}"]{cb}')

        for line in pf.readlines():
            s = line.strip()
            if s == '':
                json_sigs.append("      { }")
                continue
            stx, sfn = tosigname(s), tosigfile(s, session)
            wave, data = parse_from_trace(sfn, tau_min, tau_len)
            if boring(wave):
                # print(f'signal {stx} is boring in {tau_min}..{tau_min+tau_len-1}!')
                # print(f'  {wave=!r}')
                continue
            
            json_bits = ['    { name: "', stx, '",\n'
                         '      wave: "', wave, '"']
            if len(data) > 0:
                json_bits += [',\n      data: ["', '","'.join(data), '"]']
            json_bits.append(' }')
            json_sigs.append("".join(json_bits))

        # CAPTION line: an unnamed signal with the name as its value.
        # TODO is there a better way to do a caption?
        wa = ['.' for tau in range(act_len)]
        wa[0] = '6'
        wave = "".join(wa)
        json_sigs.append(f'    {ob} name: "", wave: "{wave}",\n      data: ["{caption}"]{cb}')

    json_sigs = ",\n".join(json_sigs)

    # at scale 1.0, 18 clocks is a nice width.
    # and gets smaller as tau_len grows from there
    # to keep the plot on one page.
    hscale = 18 / act_len
    json_string = "\n".join([
        "{ signal:",
        "  [",
        json_sigs,
        f"  ], config: {ob} hscale: {hscale}{cb}",
        "}"])

    # print(f'plotting {fbase}: {act_len=} {hscale=}')
    # print(json_string)
    draw_from_json_string(json_string, fbase)

def timing_main(args):
    for arg in args:
        try:
            draw_from_json_file(arg)
        except FileNotFoundError as e:
            print(f'{e.strerror}: {e.filename!r}')

if __name__ == "__main__":
    import sys
    timing_main(sys.argv[1:])
