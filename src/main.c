#include "main.h"
#include "headers.h"
#include "support.h"

int main(int argc, char **argv)
{
    main_post();

    for (int argi = 1; argi < argc; ++argi) {
        if (!strcmp("bist", argv[argi]))
            return main_bist(argc - argi, argv + argi);
        if (!strcmp("bench", argv[argi]))       // no bist coverage
            return main_bench(argc - argi, argv + argi);        // no bist coverage

        FAIL("unrecognized request: '%s'", argv[argi]); // no bist coverage
    }

    return 0;           // no bist coverage
}
