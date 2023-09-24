#include "timing.h"
#include "clock.h"

// timing: check simulated signal timing
//
// This facility verifies that the elapsed time from a start call to a
// final call is within a specified range. The duration of the interval
// is checked when the end of an interval is noted, and the start of the
// interval is present. The timing checker will terminate the run if it
// sees an interval with a bad duration.
//
// A summary can be printed for each checker, and a final check can be
// made that the range of values, converted to nanoseconds, is within the
// originally requested range, as the runtime checks are done in terms of
// TAU counts with limits computed from the requested durations.

// timing_invar(t): verify the invariants for a Timing.
// - we have a t, and a name, and the name is not blank.
// - if count is nonzero, then min does not exceed max.

void timing_invar(Timing t)
{
    assert(t);
    assert(t->name);
    assert(t->name[0]);

    if (t->count > 0)
        assert(t->min <= t->max);
}

// timing_init(t, name, lb_ns, ub_ns): establish a Timing.
// - name must be given and must not be blank.
// - lb_ns is the lower bound, in nanoseconds, or zero for unchecked.
// - ub_ns is the upper bound, in nanoseconds, or zero for unchecked.
//
// bounds are converted to TAU counts, carefully rounding the lower
// bound upward and the upper bound downward. Note that the computation
// of the lower bound may be an integer plus a rounding error, and the
// upper bound may be an integer minus a rounding error, so we convert
// first to float which pulls those cases to the integer.

void timing_init(Timing t, Cstr name, Tau lb_ns, Tau ub_ns)
{
    assert(t);
    assert(name);
    assert(name[0]);

    t->name = name;
    t->lb_ns = lb_ns;
    t->ub_ns = ub_ns;

    // The conversion from integer ns to floating point counts
    // could pick up a tiny rounding error: specifically, at
    // a 18.00 MHz clock, the 500 ns duration converts to a
    // correct 9 plus a unwanted tiny (1e-16 ish) error.
    // convert to float and back to trim this off.

    t->lb = (Tau)ceil((float)clock_ns_to_tau(lb_ns));
    t->ub = (Tau)floor((float)clock_ns_to_tau(ub_ns));

    t->min = 0;
    t->max = 0;
    t->count = 0;
    t->t0 = -1;

    timing_invar(t);
}

// timing_start(t): mark the start of a possible interval.
// This function is timing critical.

void timing_start(Timing t)
{
    t->t0 = TAU;
}

// timing_cancel(t): cancel the timing interval.
// This function is timing critical.

void timing_cancel(Timing t)
{
    t->t0 = -1;

    // TODO write a test that passes only if timing_cancel works.
}

// timing_final(t): mark the endof a possible interval.
// if a start was marked and not cancelled, then the checks
// are applied to it.
// This function is timing critical.
//
// Comparisons and computations are done in TAU counts.
// if this is the first check, ignore prior min and max values.

void timing_final(Timing t)
{
    if (t->t0 < 0)
        return;
    Tau                 dt = TAU - t->t0;
    if (t->lb > 0)
        assert(dt >= t->lb);
    if (t->ub > 0)
        assert(dt <= t->ub);
    if (t->count == 0) {
        t->min = dt;
        t->max = dt;
    } else {
        if (t->min > dt)
            t->min = dt;
        if (t->max < dt)
            t->max = dt;
    }
    t->count++;
    t->t0 = -1;
}

// timing_print(t): print a summary of the timing.
// prints the observed range of durations of the interval
// in TAU counts and in microseconds (to the nanosecond);
// prints the allowed range, substituting "unchecked" for
// bounds that were set to zero initially.

void timing_print(Timing t)
{
    printf("Timing %s:\n", t->name);
    if (t->count <= 0) {
        printf("  -- no samples observed.\n");
        return;
    }
    printf("  %9lld samples observed.\n", (long long)t->count);

    printf("  %9lld .. %9lld TAU observed interval\n", (long long)t->min, (long long)t->max);

    if (t->lb) {
        if (t->ub) {
            printf("  %9lld .. %9lld TAU expected interval\n",
                   (long long)t->lb, (long long)t->ub);
        } else {
            printf("  %9lld .. %9s TAU expected interval\n", (long long)t->lb, "unbounded");
        }
    } else {
        if (t->ub) {
            printf("  %9s .. %9lld TAU expected interval\n", "unbounded", (long long)t->ub);
        }
    }

    printf("  %9.3f .. %9.3f μs observed interval\n",
           clock_tau_to_ns(t->min) / 1000.0, clock_tau_to_ns(t->max) / 1000.0);

    if (t->lb_ns) {
        if (t->ub_ns) {
            printf("  %9.3f .. %9.3f μs expected interval\n",
                   t->lb_ns / 1000.0, t->ub_ns / 1000.0);
        } else {
            printf("  %9.3f .. %9s μs expected interval\n", t->lb_ns / 1000.0, "unbounded");
        }
    } else {
        if (t->ub_ns) {
            printf("  %9s .. %9.3f μs expected interval\n", "unbounded", t->ub_ns / 1000.0);
        }
    }
}

// timing_check(t): final timing check.
// Will throw an assertion failure if the range of durations
// in nanoseconds exceeds the original limit values.
// If this occurs, then the code converting the bounds to TAU
// count units has a bug.

void timing_check(Timing t)
{
    if (t->count > 0) {
        if (t->lb > 0)
            assert(t->min >= t->lb);
        if (t->ub > 0)
            assert(t->max <= t->ub);
    }
}

// test code below here.

static Edge         milliclock;
static Edge         strobe;

// share the actual timing objects.
static Timing       t0a;
static Timing       t0b;
static Timing       tm;
static Timing       t23;
static Timing       tmu;
static Timing       t23l;

static void         timing_test_init();
static void         drive_for_9_18_27(Edge);
static void         drive_for_2_3(Edge);
static void         just_inc(int *);

static void         bench_timing(void *);

// timing_post: Power-On Self Test for the Timing code
//
// This function should be called every time the program starts
// before any code relies on Timing not being completely broken.

void timing_post()
{
    timing_test_init();

    // verify that a zero bound input is registered
    // as zero limits internally (in case floating point
    // computations get "ceil" to something bad).

    timing_init(t0a, "observe milliclock", 0, 0);
    timing_invar(t0a);
    ASSERT_EQ_integer(0, t0a->lb_ns);
    ASSERT_EQ_integer(0, t0a->ub_ns);
    ASSERT_EQ_integer(0, t0a->lb);
    ASSERT_EQ_integer(0, t0a->ub);
    ASSERT_EQ_integer(0, t0a->count);
    ASSERT_EQ_integer(-1, t0a->t0);

    EDGE_FIRST_ON_RISE(milliclock, timing_start, t0a);
    EDGE_FIRST_ON_FALL(milliclock, timing_final, t0a);

    timing_init(t0b, "observe strobe", 0, 0);
    timing_invar(t0b);
    ASSERT_EQ_integer(0, t0b->lb_ns);
    ASSERT_EQ_integer(0, t0b->ub_ns);
    ASSERT_EQ_integer(0, t0b->lb);
    ASSERT_EQ_integer(0, t0b->ub);
    ASSERT_EQ_integer(0, t0b->count);
    ASSERT_EQ_integer(-1, t0b->t0);

    EDGE_FIRST_ON_RISE(strobe, timing_start, t0b);
    EDGE_FIRST_ON_FALL(strobe, timing_final, t0b);

    // verify that a 500ns to 1500ns bound becomes the proper
    // number of units.
    timing_init(tm, "milliclock check", 500, 1500);
    timing_invar(tm);
    ASSERT_EQ_integer(500, tm->lb_ns);
    ASSERT_EQ_integer(1500, tm->ub_ns);
    ASSERT_EQ_integer(9, tm->lb);
    ASSERT_EQ_integer(27, tm->ub);
    ASSERT_EQ_integer(0, tm->count);
    ASSERT_EQ_integer(-1, tm->t0);

    EDGE_FIRST_ON_RISE(milliclock, timing_start, tm);
    EDGE_FIRST_ON_FALL(milliclock, timing_final, tm);

    // verify that a 100ns to 20ns bound becomes the proper
    // number of units.
    timing_init(t23, "strobe check", 100, 200);
    timing_invar(t23);
    ASSERT_EQ_integer(100, t23->lb_ns);
    ASSERT_EQ_integer(200, t23->ub_ns);
    ASSERT_EQ_integer(2, t23->lb);
    ASSERT_EQ_integer(3, t23->ub);
    ASSERT_EQ_integer(0, t23->count);
    ASSERT_EQ_integer(-1, t23->t0);

    EDGE_FIRST_ON_RISE(strobe, timing_start, t23);
    EDGE_FIRST_ON_FALL(strobe, timing_final, t23);

    // verify that a 500ns to 1500ns bound becomes the proper
    // number of units.
    timing_init(tmu, "milliclock max", 0, 1500);
    timing_invar(tmu);

    ASSERT_EQ_integer(0, tmu->lb_ns);
    ASSERT_EQ_integer(1500, tmu->ub_ns);
    ASSERT_EQ_integer(0, tmu->lb);
    ASSERT_EQ_integer(27, tmu->ub);
    ASSERT_EQ_integer(0, tmu->count);
    ASSERT_EQ_integer(-1, tmu->t0);

    EDGE_FIRST_ON_RISE(milliclock, timing_start, tmu);
    EDGE_FIRST_ON_FALL(milliclock, timing_final, tmu);

    // verify that a 100ns to 20ns bound becomes the proper
    // number of units.
    timing_init(t23l, "strobe min", 100, 0);
    timing_invar(t23l);
    ASSERT_EQ_integer(100, t23l->lb_ns);
    ASSERT_EQ_integer(0, t23l->ub_ns);
    ASSERT_EQ_integer(2, t23l->lb);
    ASSERT_EQ_integer(0, t23l->ub);
    ASSERT_EQ_integer(0, t23l->count);
    ASSERT_EQ_integer(-1, t23l->t0);

    EDGE_FIRST_ON_RISE(strobe, timing_start, t23l);
    EDGE_FIRST_ON_FALL(strobe, timing_final, t23l);

    // all things are connected and subscribed,
    // run for 900 TAU units.

    clock_run_until(TAU + 900);

    timing_check(t0a);
    timing_check(t0b);
    timing_check(tm);
    timing_check(t23);
    timing_check(tmu);
    timing_check(t23l);
}

// timing_bist: Power-On Self Test for the Timing code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Timing code is
// not working correctly.

void timing_bist()
{
    timing_test_init();

    // verify that a zero bound input is registered
    // as zero limits internally (in case floating point
    // computations get "ceil" to something bad).

    timing_init(t0a, "observe milliclock", 0, 0);
    timing_invar(t0a);
    ASSERT_EQ_integer(0, t0a->lb_ns);
    ASSERT_EQ_integer(0, t0a->ub_ns);
    ASSERT_EQ_integer(0, t0a->lb);
    ASSERT_EQ_integer(0, t0a->ub);
    ASSERT_EQ_integer(0, t0a->count);
    ASSERT_EQ_integer(-1, t0a->t0);

    EDGE_FIRST_ON_RISE(milliclock, timing_start, t0a);
    EDGE_FIRST_ON_FALL(milliclock, timing_final, t0a);

    timing_init(t0b, "observe strobe", 0, 0);
    timing_invar(t0b);
    ASSERT_EQ_integer(0, t0b->lb_ns);
    ASSERT_EQ_integer(0, t0b->ub_ns);
    ASSERT_EQ_integer(0, t0b->lb);
    ASSERT_EQ_integer(0, t0b->ub);
    ASSERT_EQ_integer(0, t0b->count);
    ASSERT_EQ_integer(-1, t0b->t0);

    EDGE_FIRST_ON_RISE(strobe, timing_start, t0b);
    EDGE_FIRST_ON_FALL(strobe, timing_final, t0b);

    // verify that a 500ns to 1500ns bound becomes the proper
    // number of units.
    timing_init(tm, "milliclock check", 500, 1500);
    timing_invar(tm);
    ASSERT_EQ_integer(500, tm->lb_ns);
    ASSERT_EQ_integer(1500, tm->ub_ns);
    ASSERT_EQ_integer(9, tm->lb);
    ASSERT_EQ_integer(27, tm->ub);
    ASSERT_EQ_integer(0, tm->count);
    ASSERT_EQ_integer(-1, tm->t0);

    EDGE_FIRST_ON_RISE(milliclock, timing_start, tm);
    EDGE_FIRST_ON_FALL(milliclock, timing_final, tm);

    // verify that a 100ns to 20ns bound becomes the proper
    // number of units.
    timing_init(t23, "strobe check", 100, 200);
    timing_invar(t23);
    ASSERT_EQ_integer(100, t23->lb_ns);
    ASSERT_EQ_integer(200, t23->ub_ns);
    ASSERT_EQ_integer(2, t23->lb);
    ASSERT_EQ_integer(3, t23->ub);
    ASSERT_EQ_integer(0, t23->count);
    ASSERT_EQ_integer(-1, t23->t0);

    EDGE_FIRST_ON_RISE(strobe, timing_start, t23);
    EDGE_FIRST_ON_FALL(strobe, timing_final, t23);

    // verify that a 500ns to 1500ns bound becomes the proper
    // number of units.
    timing_init(tmu, "milliclock max", 0, 1500);
    timing_invar(tmu);

    ASSERT_EQ_integer(0, tmu->lb_ns);
    ASSERT_EQ_integer(1500, tmu->ub_ns);
    ASSERT_EQ_integer(0, tmu->lb);
    ASSERT_EQ_integer(27, tmu->ub);
    ASSERT_EQ_integer(0, tmu->count);
    ASSERT_EQ_integer(-1, tmu->t0);

    EDGE_FIRST_ON_RISE(milliclock, timing_start, tmu);
    EDGE_FIRST_ON_FALL(milliclock, timing_final, tmu);

    // verify that a 100ns to 20ns bound becomes the proper
    // number of units.
    timing_init(t23l, "strobe min", 100, 0);
    timing_invar(t23l);
    ASSERT_EQ_integer(100, t23l->lb_ns);
    ASSERT_EQ_integer(0, t23l->ub_ns);
    ASSERT_EQ_integer(2, t23l->lb);
    ASSERT_EQ_integer(0, t23l->ub);
    ASSERT_EQ_integer(0, t23l->count);
    ASSERT_EQ_integer(-1, t23l->t0);

    EDGE_FIRST_ON_RISE(strobe, timing_start, t23l);
    EDGE_FIRST_ON_FALL(strobe, timing_final, t23l);

    // all things are connected and subscribed,
    // run for 900 TAU units.

    clock_run_until(TAU + 900);

    timing_print(t0a);
    timing_print(t0b);
    timing_print(tm);
    timing_print(t23);
    timing_print(tmu);
    timing_print(t23l);

    timing_check(t0a);
    timing_check(t0b);
    timing_check(tm);
    timing_check(t23);
    timing_check(tmu);
    timing_check(t23l);
}

// timing_bench: performance verification for the Timing code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Timing.

void timing_bench()
{
    timing_test_init();

    // verify that a zero bound input is registered
    // as zero limits internally (in case floating point
    // computations get "ceil" to something bad).

    timing_init(t0a, "observe milliclock", 0, 0);

    EDGE_FIRST_ON_RISE(milliclock, timing_start, t0a);
    EDGE_FIRST_ON_FALL(milliclock, timing_final, t0a);

    timing_init(t0b, "observe strobe", 0, 0);

    EDGE_FIRST_ON_RISE(strobe, timing_start, t0b);
    EDGE_FIRST_ON_FALL(strobe, timing_final, t0b);

    // verify that a 500ns to 1500ns bound becomes the proper
    // number of units.
    timing_init(tm, "milliclock check", 500, 1500);

    EDGE_FIRST_ON_RISE(milliclock, timing_start, tm);
    EDGE_FIRST_ON_FALL(milliclock, timing_final, tm);

    // verify that a 100ns to 20ns bound becomes the proper
    // number of units.
    timing_init(t23, "strobe check", 100, 200);

    EDGE_FIRST_ON_RISE(strobe, timing_start, t23);
    EDGE_FIRST_ON_FALL(strobe, timing_final, t23);

    // verify that a 500ns to 1500ns bound becomes the proper
    // number of units.
    timing_init(tmu, "milliclock max", 0, 1500);

    EDGE_FIRST_ON_RISE(milliclock, timing_start, tmu);
    EDGE_FIRST_ON_FALL(milliclock, timing_final, tmu);

    // verify that a 100ns to 20ns bound becomes the proper
    // number of units.
    timing_init(t23l, "strobe min", 100, 0);

    EDGE_FIRST_ON_RISE(strobe, timing_start, t23l);
    EDGE_FIRST_ON_FALL(strobe, timing_final, t23l);

    // all things are connected and subscribed,
    // run for 900 TAU units.

    double              dt = RTC_ELAPSED(bench_timing, 0);

    Tau                 samples =
      t0a->count + t0b->count + tm->count + t23->count + tmu->count + t23l->count;

    BENCH_TOP("timing");
    BENCH_VAL(dt / samples);
    BENCH_END();

    timing_check(t0a);
    timing_check(t0b);
    timing_check(tm);
    timing_check(t23);
    timing_check(tmu);
    timing_check(t23l);
}

static void bench_timing(void *arg)
{
    (void)arg;

    t0a->count = 0;
    t0b->count = 0;
    tm->count = 0;
    t23->count = 0;
    tmu->count = 0;
    t23l->count = 0;

    clock_run_until(TAU + 90000);
}

static int          mc_rise_ct[1];
static int          st_rise_ct[1];
static int          mc_fall_ct[1];
static int          st_fall_ct[1];

static void timing_test_init()
{
    // run 18.00 MHz to be able to test the KNOWN floating point gotcha
    // when converting 500 ns to TAU units at 18 MHz.
    clock_init(18000000);

    // Create the edges we use for testing, hook up some
    // service functions on their rising and falling edges,
    // then attach the system clock to drive them.

    EDGE_INIT(milliclock, 0);
    EDGE_INIT(strobe, 0);

    *mc_rise_ct = 0;
    EDGE_ON_RISE(milliclock, just_inc, mc_rise_ct);

    *st_rise_ct = 0;
    EDGE_ON_RISE(strobe, just_inc, st_rise_ct);

    *mc_fall_ct = 0;
    EDGE_ON_FALL(milliclock, just_inc, mc_fall_ct);

    *st_fall_ct = 0;
    EDGE_ON_FALL(strobe, just_inc, st_fall_ct);

    EDGE_ON_RISE(CLOCK, drive_for_9_18_27, milliclock);
    EDGE_ON_RISE(CLOCK, drive_for_2_3, strobe);
}

static void drive_for_9_18_27(Edge e)
{
    static int          phase = 0;
    switch (phase++) {
      default:
          break;
      case 10:
          edge_hi(e);
          break;
      case 19:
          edge_lo(e);
          break;
      case 20:
          edge_hi(e);
          break;
      case 38:
          edge_lo(e);
          break;
      case 40:
          edge_hi(e);
          break;
      case 67:
          edge_lo(e);
          break;
      case 90:
          phase = 1;
          break;
    }
}

static void drive_for_2_3(Edge e)
{
    static int          phase = 0;
    switch (phase++) {
      default:
          break;
      case 2:
          edge_hi(e);
          break;
      case 4:
          edge_lo(e);
          break;
      case 5:
          edge_hi(e);
          break;
      case 8:
          edge_lo(e);
          break;
      case 9:
          phase = 1;
          break;
    }
}

static void just_inc(int *ctr)
{
    ++*ctr;
}
