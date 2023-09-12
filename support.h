#pragma once
#include "headers.h"

// === === === === === === === === === === === === === === === ===
//                          UTILITY MACROS
// === === === === === === === === === === === === === === === ===

// Macros to provide visible annotations on stdout
// for entering and exiting a function, used mainly
// in Built-In Self Test executives.

#define	PRINT_TOP() printf("\n=== === TOP %s\n", __func__)
#define	PRINT_END() printf("\n*** *** end %s\n", __func__)

// === === === === === === === === === === === === === === === ===
//                        UTILITY DATA TYPES
// === === === === === === === === === === === === === === === ===

typedef const char *Str;                        // C string (immutable with \0 at end)

// Elapsed time in arbitrary units, with sufficient range to store
// a nanosecond elapsed time of days ... weeks ... years even.

typedef long long Tau;

#define TAU_MIN		LLONG_MIN
#define TAU_MAX		LLONG_MAX

// === === === === === === === === === === === === === === === ===
//                          UTILITY FUNCTIONS
// === === === === === === === === === === === === === === === ===

// printf but to a freshly allocated Str.
extern Str          format(Str fmt, ...);

// === === === === === === === === === === === === === === === ===
//                          DEBUG SUPPORT
// === === === === === === === === === === === === === === === ===

extern int          _stub(int fatal, Str file, int line, Str func, Str fmt,
                          ...);
#define STUB(...)	(_stub(0, __FILE__, __LINE__, __func__, __VA_ARGS__))
#define FAIL(...)	(_stub(1, __FILE__, __LINE__, __func__, __VA_ARGS__))

extern int          _fail(Str file, int line, Str func, Str cond, Str fmt, ...);

#define	ASSERT(cond, ...)	if (0 != (cond)) ; else _fail(__FILE__, __LINE__, __func__, #cond, __VA_ARGS__)

#define	ASSERT_EQ_integer(expected, observed)                   \
    if ((expected) == (observed))                               \
        ;                                                       \
    else                                                        \
        _fail(__FILE__, __LINE__, __func__,                     \
              "" #observed " == " #expected,                    \
              "Equality Comparison Failed\n"                    \
              "  Expected value: %llu (%s)\n"                   \
              "  Observed value: %llu (%s)\n"                   \
              , ((long long)(expected)), #expected              \
              , ((long long)(observed)), #observed              \
            )

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
