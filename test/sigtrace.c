#include "common/sigtrace.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include <errno.h>      // Macros reporting error conditions
#include <stdlib.h>     // General utilities
#include <string.h>     // String handling
#include <sys/stat.h>
#include <sys/types.h>

static void         sigtrace_putc(SigTrace trc, char ch);

#define TAU_SKIP_MAX	10
#define TAU_LINE_MAX	72

void sigtrace_init(SigTrace trc, SigSess sess, Cstr name)
{
    assert(trc);
    assert(sess);

    trc->sess = sess;
    trc->name = name;
    // trc->edge = 0;
    // trc->addr = 0;
    // trc->data = 0;
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

void sigtrace_plot(SigTrace trc, SigPlot plot)
{
    if (NULL == trc) {
        fprintf(plot->fp, "\n");
        return;
    }

    fprintf(plot->fp, "%s\n", trc->name);
}

void sigtrace_fini(SigTrace trc)
{
    fprintf(trc->fp, "\n");
    fclose(trc->fp);
    trc->fp = 0;
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
