#include "subs.h"
#include "support.h"

void subs_invar(Subs s)
{
    assert(s);
    assert(s->cap >= s->len);
    if (s->cap > 0) {
        assert(s->list);
        if (s->len > 0) {
            for (int i = 0; i < s->len; ++i) {
                step_invar(s->list + i);
            }
        }
    }
}

void subs_init(Subs s)
{
    s->list = 0;
    s->cap = 0;
    s->len = 0;
    subs_invar(s);
}

void subs_add(Subs s, StepFp fp, StepAp ap)
{
    (void)fp;
    (void)ap;

    subs_invar(s);

    int                 cap = s->cap;
    int                 len = s->len;
    pStep               list = s->list;

    if (len == cap) {
        cap = (cap < 8) ? 8 : 2 * cap;
        void               *copy = realloc(list, cap * sizeof(list[0]));
        ASSERT(copy, "unable to realloc %p to %d entries\n", (void *)list, cap);
        list = copy;
        s->list = list;
        s->cap = cap;
        subs_invar(s);
    }

    ASSERT(len < cap,
           "after expansion, cap %d not big enough to expand to %d items", cap,
           len);

    list[len].fp = fp;
    list[len].ap = ap;
    s->len = len + 1;
    subs_invar(s);
}

void subs_run(Subs s)
{
    for (int i = 0; i < s->len; ++i) {
        step_run(s->list + i);
    }
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void         subs_fn1(int *);
static void         subs_fn2(int *);
static void         subs_fn3(int *);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void subs_post()
{
    Subs                s;
    subs_init(s);
    ASSERT_EQ_integer(0, s->len);

    const int           start_val = 1337;

    int                 subs_args[1] = { start_val };

    SUBS_ADD(s, subs_fn1, subs_args + 0);

    subs_run(s);

    ASSERT_EQ_integer(start_val + 1, subs_args[0]);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void subs_bist()
{
    Subs                s;
    subs_init(s);
    ASSERT_EQ_integer(0, s->len);

    const int           start_val = 1337;

    int                 subs_args[3] = { start_val, start_val, start_val };

    SUBS_ADD(s, subs_fn1, subs_args + 0);
    SUBS_ADD(s, subs_fn2, subs_args + 1);
    SUBS_ADD(s, subs_fn3, subs_args + 2);

    subs_run(s);
    subs_run(s);
    subs_run(s);

    ASSERT_EQ_integer(start_val + 3, subs_args[0]);
    ASSERT_EQ_integer(start_val + 30, subs_args[1]);
    ASSERT_EQ_integer(start_val + 300, subs_args[2]);
}

// === === === === === === === === === === === === === === === ===
//                   BENCHMARK THE SUBS FACILITY
// === === === === === === === === === === === === === === === ===

void subs_bench()
{
    Subs                s;
    subs_init(s);

#define TEST_LEN	16

    unsigned           *subs_args = calloc(TEST_LEN, sizeof(unsigned));
    assert(subs_args);

    for (int i = 0; i < TEST_LEN; ++i)
        SUBS_ADD(s, subs_fn1, subs_args + i);

    Step                bench_subs = STEP_INIT(subs_run, s);

    double              dt = step_elapsed(bench_subs);

    fprintf(stderr, "BENCH: %8.3f ns per subs_run() call\n", dt);
    fprintf(stderr, "  ...  %8.3f ns per step_run() call via the subs list\n",
            dt / TEST_LEN);
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void subs_fn1(int *arg)
{
    *arg += 1;
}
static void subs_fn2(int *arg)
{
    *arg += 10;
}
static void subs_fn3(int *arg)
{
    *arg += 100;
}
