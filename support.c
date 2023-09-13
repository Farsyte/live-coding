#include "support.h"

// printf but to a freshly allocated Str.
Cstr format(Cstr fmt, ...)
{
    va_list             ap;
    char               *result;
    int                 length;
    int                 actual;

    // int vsnprintf(char *str, size_t size, const char *format, va_list ap);
    //
    //    The function vsnprintf() does not write more than size
    //    bytes, including the terminating NUL. If the output was
    //    truncated due to this limit, then the return value is the
    //    number of characters (EXCLUDING the terminating null byte)
    //    which WOULD HAVE BEEN written to the final string if enough
    //    space had been available. Thus, a return value of size or more
    //    means that the output was truncated.

    // va_start(ap, fmt);
    // vfprintf(stderr, fmt, ap);
    // va_end(ap);

    va_start(ap, fmt);
    length = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

    length += 1;        // add the terminating NUL
    result = malloc(length);

    va_start(ap, fmt);
    actual = vsnprintf(result, length, fmt, ap);
    va_end(ap);

    assert(actual <= length);
    return result;
}

// generate a useful message that next-error can parse, maybe abort.
extern int _stub(int fatal, Cstr file, int line, Cstr func, Cstr fmt, ...)
{

    fprintf(stderr, "%s:%d: (in %s): ", file, line, func);

    va_list             ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");
    if (fatal)
        abort();

    return 0;
}

// generate test failure report that next-error can parse, maybe abort.
extern int _fail(Cstr file, int line, Cstr func, Cstr cond, Cstr fmt, ...)
{
    fprintf(stderr, "%s:%d: (in %s): test condition failed\n    (%s)\n", file,
            line, func, cond);

    va_list             ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");
    abort();

    return 0;
}
