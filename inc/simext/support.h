#pragma once
#include <errno.h>
#include <limits.h>     // Ranges of integer types
#include <stdint.h>     // Fixed-width integer types
#include <stdio.h>      // Input/output
#include <stdlib.h>
#include <string.h>     // String handling

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

// Bit: storage for a single bit of data.
// type definition is the same for all simulated items.
typedef uint8_t    *pBit, Bit;

// Byte: storage for a single byte of data (8 bits).
// type definition is the same for all simulated items.
typedef uint8_t    *pByte, Byte;

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

#define FIN(var, func, ...)                                             \
    int var = func(__VA_ARGS__);                                        \
    if (var >= 0) ; else                                                \
        FAIL("\n\tint %s = %s(%s)\n\tindicates error %d: %s",           \
             "" #var, #func, #__VA_ARGS__,                              \
             errno, strerror(errno))

extern int          _fail(Cstr file, int line, Cstr func, Cstr cond, Cstr fmt, ...);

#define	ASSERT(cond, ...)                                               \
    do {                                                                \
        if (0 != (cond))                                                \
            ;                                                           \
        else                                                            \
            _fail(__FILE__, __LINE__, __func__,                         \
                  "" #cond, __VA_ARGS__);                               \
    } while (0)

#define	ASSERT_OP_integer(__exp_expr, op, __obs_expr)                   \
    do {                                                                \
        long long __exp = ((long long)(__exp_expr));                    \
        long long __obs = ((long long)(__obs_expr));                    \
        if ((__obs) op (__exp))                                         \
            ;                                                           \
        else                                                            \
            _fail(__FILE__, __LINE__, __func__,                         \
                  "" #__obs_expr " " #op " " #__exp_expr,               \
                  "Comparison Failed:\n"                                \
                  "  __Exp_Expr value: %2s %lld == 0x%X (%s)\n"         \
                  "  __Obs_Expr value:    %lld == 0x%X (%s)\n"          \
                  , #op                                                 \
                  , __exp, __exp, #__exp_expr                           \
                  , __obs, __obs, #__obs_expr                           \
                );                                                      \
    } while (0)

#define	ASSERT_EQ_integer(__exp_expr, __obs_expr)                       \
    ASSERT_OP_integer(__exp_expr, ==, __obs_expr)

#define	ASSERT_NE_integer(__exp_expr, __obs_expr)                       \
    ASSERT_OP_integer(__exp_expr, !=, __obs_expr)

#define	ASSERT_LT_integer(__exp_expr, __obs_expr)                       \
    ASSERT_OP_integer(__exp_expr, <, __obs_expr)

#define	ASSERT_GT_integer(__exp_expr, __obs_expr)                       \
    ASSERT_OP_integer(__exp_expr, >, __obs_expr)

#define	ASSERT_LE_integer(__exp_expr, __obs_expr)                       \
    ASSERT_OP_integer(__exp_expr, <=, __obs_expr)

#define	ASSERT_GE_integer(__exp_expr, __obs_expr)                       \
    ASSERT_OP_integer(__exp_expr, >=, __obs_expr)

#define	ASSERT_EQ_string(__exp_expr, __obs_expr)                        \
    do {                                                                \
        Cstr __exp = ((Cstr)(__exp_expr));                              \
        Cstr __obs = ((Cstr)(__obs_expr));                              \
        if (!strcmp(__obs, __exp))                                      \
            ;                                                           \
        else                                                            \
            _fail(__FILE__, __LINE__, __func__,                         \
                  "\"" #__obs_expr "\" == \"" #__exp_expr "\"",         \
                  "String Equaltiy Comparison Failed:\n"                \
                  "  __Exp_Expr value: \"%s\"\n"                        \
                  "  __Obs_Expr value: \"%s\"\n"                        \
                  , __exp, __obs                                        \
                );                                                      \
    } while (0)

#define	ASSERT_OP_pointer(__exp_expr, op, __obs_expr)                   \
    do {                                                                \
        void * __exp = ((void *)(__exp_expr));                          \
        void * __obs = ((void *)(__obs_expr));                          \
        if ((__obs) op (__exp))                                         \
            ;                                                           \
        else                                                            \
            _fail(__FILE__, __LINE__, __func__,                         \
                  "" #__obs_expr " " #op " " #__exp_expr,               \
                  "Pointer Comparison Failed:\n"                        \
                  "  __Exp_Expr value: %2s %lld (%s)\n"                 \
                  "  __Obs_Expr value:    %lld (%s)\n"                  \
                  , #op                                                 \
                  , __exp, #__exp_expr                                  \
                  , __obs, #__obs_expr                                  \
                );                                                      \
    } while (0)

#define	ASSERT_NE_pointer(__exp_expr, __obs_expr)                       \
    ASSERT_OP_pointer(__exp_expr, !=, __obs_expr)

#define	ASSERT_NZ_pointer(__obs_expr)                                   \
    ASSERT_NE_pointer(NULL, __obs_expr)

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

typedef void        (BenchFn) (void *);

extern double       rtc_elapsed(BenchFn *, void *);
#define RTC_ELAPSED(fn, ap)	(rtc_elapsed((BenchFn *)(fn), (void *)(ap)))

#define BENCH_TOP(item)	fprintf(stderr, "BENCH: %-24s", item)
#define BENCH_VAL(dt)	fprintf(stderr, "%5.1f ", (double)(dt))
#define BENCH_END()	fprintf(stderr, "\n")
