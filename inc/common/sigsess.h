#pragma once
#include "common/support.h"

// sigsess: signal trace per-session data

typedef struct sSigSess {
    Cstr                name;
    Cstr                dirname;
}                  *pSigSess, SigSess[1];

extern void         sigsess_init(SigSess sess, Cstr name);
extern void         sigsess_fini(SigSess sess);
