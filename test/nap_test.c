#include "simext/nap.h"
#include "simext/support.h"

void nap_post()
{
    nap_pend = 0;
    nap_total = 0;

    nap(0);
    ASSERT_GE_integer(1000000, nap_total);
    ASSERT_LE_integer(0, nap_pend);
    nap(0);
    ASSERT_GE_integer(2000000, nap_total);
    ASSERT_LE_integer(0, nap_pend);
}

void nap_bist()
{
    Tau                 nt[100];
    Tau                 np[100];

    nap_init();

    for (int i = 0; i < 100; ++i) {
        nap(0);
        nt[i] = nap_total;
        np[i] = nap_pend;
    }
    for (int i = 0; i < 100; ++i) {
        ASSERT_GE_integer(i * 1000000, nt[i]);
        ASSERT_LE_integer(0, np[i]);
    }
}
