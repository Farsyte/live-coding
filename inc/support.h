#pragma once
#include "headers.h"

// === === === === === === === === === === === === === === === ===
//                          UTILITY MACROS
// === === === === === === === === === === === === === === === ===

// Macros to provide visible annotations on stdout
// for entering and exiting a function, used mainly
// in Built-In Self Test executives.

#define	PRINT_TOP() STUB("*** *** TOP")
#define	PRINT_END() STUB("*** *** end")

// === === === === === === === === === === === === === === === ===
//                        UTILITY DATA TYPES
// === === === === === === === === === === === === === === === ===

// C string -- immutable, with ACII NUL ('\0') at end.
typedef const char *Cstr;

// Elapsed time in arbitrary units, with sufficient range to store
// a nanosecond elapsed time of days ... weeks ... years even.

typedef long long Tau;

#define TAU_MIN		LLONG_MIN
#define TAU_MAX		LLONG_MAX

// The "support" facility owns the global TAU singleton, which
// represents the current simulated time in undefined units with
// undefined zero point.
//
// TAU is set by "stepat" to the TAU for the step being run.

extern Tau          TAU;

// === === === === === === === === === === === === === === === ===
//                          UTILITY FUNCTIONS
// === === === === === === === === === === === === === === === ===

// printf but to a freshly allocated Cstr.
extern Cstr         format(Cstr fmt, ...);

// === === === === === === === === === === === === === === === ===
//                          DEBUG SUPPORT
// === === === === === === === === === === === === === === === ===

extern int          _stub(int fatal, Cstr file, int line, Cstr func, Cstr fmt, ...);
#define STUB(...)	(_stub(0, __FILE__, __LINE__, __func__, __VA_ARGS__))
#define FAIL(...)	(_stub(1, __FILE__, __LINE__, __func__, __VA_ARGS__))

extern int          _fail(Cstr file, int line, Cstr func, Cstr cond, Cstr fmt, ...);

#define	ASSERT(cond, ...)                       \
    do {                                        \
        if (0 != (cond))                        \
            ;                                   \
        else                                    \
            _fail(__FILE__, __LINE__, __func__, \
                  "" #cond, __VA_ARGS__);       \
    } while (0)

#define	ASSERT_OP_integer(expected, op, observed)               \
    do {                                                        \
        long long exp = ((long long)(expected));                \
        long long obs = ((long long)(observed));                \
        if ((obs) op (exp))                                     \
            ;                                                   \
        else                                                    \
            _fail(__FILE__, __LINE__, __func__,                 \
                  "" #observed " " #op " " #expected,           \
                  "Comparison Failed:\n"                        \
                  "  Expected value: %2s %lld == 0x%X (%s)\n"   \
                  "  Observed value:    %lld == 0x%X (%s)\n"    \
                  , #op                                         \
                  , exp, exp, #expected                         \
                  , obs, obs, #observed                         \
                );                                              \
    } while (0)

#define	ASSERT_EQ_integer(expected, observed)                   \
    ASSERT_OP_integer(expected, ==, observed)

#define	ASSERT_NE_integer(expected, observed)                   \
    ASSERT_OP_integer(expected, !=, observed)

#define	ASSERT_LT_integer(expected, observed)                   \
    ASSERT_OP_integer(expected, <, observed)

#define	ASSERT_GT_integer(expected, observed)                   \
    ASSERT_OP_integer(expected, >, observed)

#define	ASSERT_LE_integer(expected, observed)                   \
    ASSERT_OP_integer(expected, <=, observed)

#define	ASSERT_GE_integer(expected, observed)                   \
    ASSERT_OP_integer(expected, >=, observed)

#define	ASSERT_EQ_string(expected, observed)                    \
    do {                                                        \
        Cstr exp = ((Cstr)(expected));                          \
        Cstr obs = ((Cstr)(observed));                          \
        if (!strcmp(obs, exp))                                  \
            ;                                                   \
        else                                                    \
            _fail(__FILE__, __LINE__, __func__,                 \
                  "\"" #observed "\" == \"" #expected "\"",     \
                  "String Equaltiy Comparison Failed:\n"        \
                  "  Expected value: \"%s\"\n"                  \
                  "  Observed value: \"%s\"\n"                  \
                  , exp, obs                                    \
                );                                              \
    } while (0)

#define	ASSERT_OP_pointer(expected, op, observed)               \
    do {                                                        \
        void * exp = ((void *)(expected));                      \
        void * obs = ((void *)(observed));                      \
        if ((obs) op (exp))                                     \
            ;                                                   \
        else                                                    \
            _fail(__FILE__, __LINE__, __func__,                 \
                  "" #observed " " #op " " #expected,           \
                  "Pointer Comparison Failed:\n"                \
                  "  Expected value: %2s %lld (%s)\n"           \
                  "  Observed value:    %lld (%s)\n"            \
                  , #op                                         \
                  , exp, #expected                              \
                  , obs, #observed                              \
                );                                              \
    } while (0)

#define	ASSERT_NE_pointer(expected, observed)                   \
    ASSERT_OP_pointer(expected, !=, observed)

#define	ASSERT_NZ_pointer(observed)                             \
    ASSERT_NE_pointer(NULL, observed)

// === === === === === === === === === === === === === === === ===
//                  SUPPORT TESTS AND TEST SUPPORT
// === === === === === === === === === === === === === === === ===

extern void         tau_post();                 // assure Tau is good
extern void         rtc_post();                 // assure RTC is good
extern void         rtc_bist();                 // check BIST at each build

// === === === === === === === === === === === === === === === ===
//             SUPPORT BENCHMARKS AND BENCHMARK SUPPORT
// === === === === === === === === === === === === === === === ===

// The rtc_ns() function returns the elapsed wall-clock host time
// since an arbitrary epoch, to nanosecond resolution.
extern Tau          rtc_ns();                   // wall time (ns) since epoch

// The rtc_prec_ns() function returns the precision of the RTC
// claimed by the host system.
extern Tau          rtc_prec_ns();              // wall time (ns) since epoch

extern void         rtc_bench();                // measure RTC facility performance

typedef void        BenchFn(void *);

extern double       rtc_elapsed(BenchFn *, void *);
#define RTC_ELAPSED(fn, ap)	(rtc_elapsed((BenchFn *)(fn), (void *)(ap)))

#define BENCH_TOP(item)	fprintf(stderr, "BENCH: %-24s", item)
#define BENCH_VAL(dt)	fprintf(stderr, "%5.1f ", (double)(dt))
#define BENCH_END()	fprintf(stderr, "\n")
