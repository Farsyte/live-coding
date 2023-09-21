#include <sys/stat.h>
#include <sys/types.h>
#include "sigtrace.h"

static void         sig_rise(SigTrace trc);
static void         sig_fall(SigTrace trc);
static void         sig_dv(SigTrace trc);
static void         sig_dz(SigTrace trc);
static void         sig_av(SigTrace trc);
static void         sig_az(SigTrace trc);

static void         sigtrace_putc(SigTrace trc, char ch);

#define TAU_SKIP_MAX	10
#define TAU_LINE_MAX	72

void sigsess_init(SigSess sess, Cstr name)
{
    assert(sess);
    assert(name);
    assert(name[0]);

    sess->name = name;
    sess->dirname = format("log/timing/%s", sess->name);

    int                 rv = mkdir(sess->dirname, 0750);
    if (rv < 0 && errno != EEXIST) {
        fprintf(stderr, "Error %d trying to create %s: %s\n",
                errno, sess->dirname, strerror(errno));
        abort();
    }
}

void sigtrace_init(SigTrace trc, SigSess sess, Cstr name)
{
    assert(trc);
    assert(sess);

    trc->sess = sess;
    trc->name = name;
    trc->edge = 0;
    trc->addr = 0;
    trc->data = 0;
    trc->next_tau = -TAU_SKIP_MAX - 1;
    trc->curr_line_len = 0;

    Cstr                sn = name;
    Cstr                path_start = sn;
    Cstr                path_end = sn;

    // if there are colons, advance past the last one.
    for (Cstr p = sn; *p; ++p)
        if (*p == ':') {
            sn = p + 1;
            path_start = name;
            path_end = sn;
        }
    // Exclude leading / and \ from the filename.
    if (*sn == '/')
        ++sn;
    if (*sn == '\\')
        ++sn;

    // Exclude trailing _ from the filename.
    Cstr                ee = sn;
    while (*ee)
        ++ee;
    if (ee > sn && ee[-1] == '_')
        --ee;

    size_t              ep = path_end - path_start;
    size_t              el = ee - sn;
    trc->filename = format("%s/%*.*s%*.*s.log", sess->dirname, ep, ep, path_start, el, el, sn);
    trc->fp = fopen(trc->filename, "w");

    if (NULL == trc->fp) {
        fprintf(stderr, "Error %d trying to create %s: %s\n",
                errno, trc->filename, strerror(errno));
        abort();
    }

}

void sigtrace_init_edge(SigTrace trc, SigSess sess, pEdge edge)
{
    assert(trc);
    assert(sess);
    edge_invar(edge);

    sigtrace_init(trc, sess, edge->name);
    trc->edge = edge;

    EDGE_ON_RISE(edge, sig_rise, trc);
    EDGE_ON_FALL(edge, sig_fall, trc);

    if (edge->value)
        sig_rise(trc);
    else
        sig_fall(trc);
}

void sigtrace_init_addr(SigTrace trc, SigSess sess, pAddr addr)
{
    assert(trc);
    assert(sess);
    addr_invar(addr);

    sigtrace_init(trc, sess, addr->name);
    trc->addr = addr;

    ADDR_ON_VALID(addr, sig_av, trc);
    ADDR_ON_Z(addr, sig_az, trc);
    sig_az(trc);
}

void sigtrace_init_data(SigTrace trc, SigSess sess, pData data)
{
    assert(trc);
    assert(sess);
    data_invar(data);

    sigtrace_init(trc, sess, data->name);
    trc->data = data;

    DATA_ON_VALID(data, sig_dv, trc);
    DATA_ON_Z(data, sig_dz, trc);
    sig_dz(trc);
}

void sigplot_init(SigPlot plot, SigSess sess, Cstr name,
                  Cstr title, Cstr caption, Tau tau_min, Tau tau_len)
{
    plot->sess = sess;
    plot->filename = format("log/timing/%s.txt", name);
    plot->fp = fopen(plot->filename, "w");
    if (NULL == plot->fp) {
        fprintf(stderr, "Error %d trying to create %s: %s\n",
                errno, plot->filename, strerror(errno));
        abort();
    }
    fprintf(plot->fp, "%s\n", title);
    fprintf(plot->fp, "%s\n", caption);
    fprintf(plot->fp, "%s\n", sess->name);
    fprintf(plot->fp, "%lld %lld\n", tau_min, tau_len);
}

void sigtrace_log(SigTrace trc, char ch, Cstr data)
{
    Tau                 skipped_tau = TAU - trc->next_tau;
    if (skipped_tau > TAU_SKIP_MAX) {
        if (trc->next_tau > 0)
            fprintf(trc->fp, "\n");
        fprintf(trc->fp, "%lld:\n", TAU);
        trc->next_tau = TAU;
        trc->curr_line_len = 0;
    }
    while (trc->next_tau < TAU)
        sigtrace_putc(trc, '.');
    sigtrace_putc(trc, ch);
    if (data && data[0]) {
        fprintf(trc->fp, "%s", data);
        trc->curr_line_len = 999;
    }
}

void sigplot_sig(SigPlot plot, SigTrace trc)
{
    if (NULL == trc) {
        fprintf(plot->fp, "\n");
        return;
    }

    fprintf(plot->fp, "%s\n", trc->name);
}

void sigplot_fini(SigPlot plot)
{
    fclose(plot->fp);
    plot->fp = 0;
}

void sigtrace_fini(SigTrace trc)
{
    fprintf(trc->fp, "\n");
    fclose(trc->fp);
    trc->fp = 0;
}

void sigsess_fini(SigSess sess)
{
    (void)sess;
}

static void sig_rise(SigTrace trc)
{
    sigtrace_log(trc, '1', 0);
}

static void sig_fall(SigTrace trc)
{
    sigtrace_log(trc, '0', 0);
}

static void sig_dv(SigTrace trc)
{
    sigtrace_log(trc, '=', format("%02X", trc->data->value));
}
static void sig_dz(SigTrace trc)
{
    sigtrace_log(trc, 'z', 0);
}
static void sig_av(SigTrace trc)
{
    sigtrace_log(trc, '=', format("%04X", trc->addr->value));
}
static void sig_az(SigTrace trc)
{
    sigtrace_log(trc, 'z', 0);
}

static void sigtrace_putc(SigTrace trc, char ch)
{
    if (trc->curr_line_len < TAU_LINE_MAX) {
        trc->curr_line_len++;
    } else {
        fprintf(trc->fp, "\n");
        trc->curr_line_len = 1;
    }
    fprintf(trc->fp, "%c", ch);
    trc->next_tau++;
}
