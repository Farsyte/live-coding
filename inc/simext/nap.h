#pragma once
#include "simext/support.h"

// nap: stop using CPU briefly, with bookkeeping.

// nap_total: actual rtc change accumulated while napping
extern Tau          nap_total;

// nap_pend: requested nap less actual nap
// negative when we have overslept (this is common)
extern Tau          nap_pend;

// nap_init: initialize (or reinitialize) the nap bookkeeping

extern void         nap_init();

// nap: stop running for a millisecond
// increments nap_total by time actually spent napping

extern void         nap();

extern void         nap_post();
extern void         nap_bist();
