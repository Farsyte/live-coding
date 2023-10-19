#include "common/sigsess.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include <errno.h>      // Macros reporting error conditions
#include <sys/stat.h>
#include <sys/types.h>
#include "common/support.h"

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

void sigsess_fini(SigSess sess)
{
    (void)sess;
}
