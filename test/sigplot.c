#include "common/sigplot.h"
#include <stdio.h>
#include <stdlib.h>
#include "simext/support.h"

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

void sigplot_fini(SigPlot plot)
{
    fclose(plot->fp);
    plot->fp = 0;
}
