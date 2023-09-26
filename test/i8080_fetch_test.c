#include "i8080_impl.h"

// i8080_fetch: manage the STATUS_FETCH machine cycle
// T1 drives PC to address, STATUS to data, and SYNC high.
// T2 increments PC, releases Data and Sync, drives DBIN
// TW waits for READY if needed
// T3 samples Data to IR, releases DBIN
// control delivered via cpu->m1t4[IR] for the T4 cycle

void i8080_fetch_post()
{
    PRINT_TOP();
    // not yet written or called
    PRINT_END();
}

void i8080_fetch_bist()
{
    PRINT_TOP();
    // not yet written or called
    PRINT_END();
}

void i8080_fetch_bench()
{
    PRINT_TOP();
    // not yet written or called
    PRINT_END();
}
