#include "common/main.h"
#include <string.h>     // String handling
#include "common/cdev.h"
#include "simext/support.h"
#include "system/VoidStar8080.h"

int main(int argc, char **argv)
{
    main_post();

    for (int argi = 1; argi < argc; ++argi) {

        if (!strcmp("bist", argv[argi]))
            return main_bist(argc - argi, argv + argi);

        if (!strcmp("bench", argv[argi]))       // no bist coverage
            return main_bench(argc - argi, argv + argi);        // no bist coverage

        if (!strcmp("cdev", argv[argi]))
            return cdev_test(argc - argi, argv + argi); // no bist coverage

        if (!strcmp("VoidStar8080", argv[argi]))
            return VoidStar8080_main(argc - argi, argv + argi); // no bist coverage

        FAIL("unrecognized request: '%s'", argv[argi]); // no bist coverage
    }

    return 0;           // no bist coverage
}
