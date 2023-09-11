#include "support.h"

static void main_post()
{

    tau_post();
    rtc_post();
}

static void main_bist()
{
    rtc_bist();
}

static void main_bench()
{
    rtc_bench();
}

int main(int argc, char **argv)
{

    (void)argc;
    (void)argv;

    main_post();
    main_bist();
    main_bench();
    return 0;
}
