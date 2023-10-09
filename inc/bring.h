#pragma once
#include "support.h"
#include "target.h"

#define BRING_CAP	255

typedef struct sBring {
    Cstr                name;

    // These are declared volatile to assure correct behavior
    // of the multithreaded simulation: writes to data must be
    // visible before writes to the prod and cons indexes.
    volatile Byte       prod;
    volatile Byte       cons;
    volatile Byte       data[BRING_CAP + 1];
}                  *pBring, Bring[1];

extern void         bring_init(Bring, Cstr);
extern void         bring_invar(const Bring);
extern void         bring_linked(Bring);

extern int          bring_empty(const Bring);
extern int          bring_full(const Bring);
extern void         bring_prod(Bring, Byte);
extern Byte         bring_cons(Bring);

extern void         bring_post();
