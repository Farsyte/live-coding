#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include <stdlib.h>     // General utilities
#include "common/subs.h"
#include "common/support.h"

// subs: a place for things to do

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

#define SUBS_WHICH_SIZE 72
static char         subs_which[SUBS_WHICH_SIZE + 1];
static int          subs_which_ct = 0;
static void         subs_which_add(char);
static void         subs_which_cap();

static void         subs_fnB(int *);
static void         subs_fn1(int *);
static void         subs_fn2(int *);
static void         subs_fn3(int *);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

// subs_post: Power-On Self Test for the Subs code
//
// This function should be called every time the program starts
// before any code relies on Subs not being completely broken.

void subs_post()
{
    Subs                s;
    subs_init(s);
    ASSERT_EQ_integer(0, s->len);

    const int           start_val = 1337;

    int                 subs_args[3] = { start_val, start_val, start_val };

    SUBS_ADD(s, subs_fn2, subs_args + 1);
    SUBS_ADD(s, subs_fn3, subs_args + 2);
    SUBS_FIRST(s, subs_fn1, subs_args + 0);

    subs_which_ct = 0;
    subs_run(s);
    subs_which_cap();

    ASSERT_EQ_string("123", subs_which);

    ASSERT_EQ_integer(start_val + 1, subs_args[0]);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

// subs_bist: Power-On Self Test for the Subs code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Subs code is
// not working correctly.

void subs_bist()
{
    Subs                s;
    subs_init(s);
    ASSERT_EQ_integer(0, s->len);

    const int           start_val = 1337;

    int                 subs_args[3] = { start_val, start_val, start_val };

    SUBS_ADD(s, subs_fn2, subs_args + 1);
    SUBS_ADD(s, subs_fn3, subs_args + 2);
    SUBS_FIRST(s, subs_fn1, subs_args + 0);

    subs_which_ct = 0;
    subs_run(s);
    subs_run(s);
    subs_run(s);
    subs_which_cap();
    ASSERT_EQ_string("123123123", subs_which);

    ASSERT_EQ_integer(start_val + 3, subs_args[0]);
    ASSERT_EQ_integer(start_val + 30, subs_args[1]);
    ASSERT_EQ_integer(start_val + 300, subs_args[2]);
}

// === === === === === === === === === === === === === === === ===
//                   BENCHMARK THE SUBS FACILITY
// === === === === === === === === === === === === === === === ===

// subs_bench: performance verification for the Subs code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Subs.

void subs_bench()
{
    Subs                s;
    subs_init(s);

#define TEST_LEN	16

    unsigned           *subs_args = calloc(TEST_LEN, sizeof(unsigned));
    assert(subs_args);

    for (int i = 0; i < TEST_LEN; ++i)
        SUBS_ADD(s, subs_fnB, subs_args + i);

    double              dt = RTC_ELAPSED(subs_run, s);

    BENCH_TOP("subs");
    BENCH_VAL(dt / TEST_LEN);
    BENCH_END();
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void subs_fnB(int *arg)
{
    *arg += 1;
}
static void subs_fn1(int *arg)
{
    *arg += 1;
    subs_which_add('1');
}
static void subs_fn2(int *arg)
{
    *arg += 10;
    subs_which_add('2');
}
static void subs_fn3(int *arg)
{
    *arg += 100;
    subs_which_add('3');
}
static void subs_which_add(char ch)
{
    if (subs_which_ct <= SUBS_WHICH_SIZE)
        subs_which[subs_which_ct++] = ch;
}
static void subs_which_cap()
{
    subs_which_add('\0');
    subs_which[SUBS_WHICH_SIZE] = '\0';
}
