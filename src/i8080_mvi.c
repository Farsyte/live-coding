#include "clock.h"
#include "i8080_impl.h"

// i8080_mvi: manage most MVI instructions

// This is one way to not write nearly the same function
// over and over.

// NAME_M2T3(R): use token-pasting to construct the name.

#define NAME_M2T3(R)	i8080_state_mviM2T3 ## R
#define DEFN_M2T3(R)	static f8080State NAME_M2T3(R)

DEFN_M2T3(A);
DEFN_M2T3(B);
DEFN_M2T3(C);
DEFN_M2T3(D);
DEFN_M2T3(E);
DEFN_M2T3(H);
DEFN_M2T3(L);

static p8080State   i8080_state_mviM2T3[8] = {
    NAME_M2T3(B),
    NAME_M2T3(C),
    NAME_M2T3(D),
    NAME_M2T3(E),
    NAME_M2T3(H),
    NAME_M2T3(L),
    0,
    NAME_M2T3(A),
};

// i8080_mvi_init: set up decoding for MVI instructions

void i8080_mvi_init(i8080 cpu)
{
    for (int ddd = 0; ddd < 8; ++ddd) {
        p8080State          m2t3 = i8080_state_mviM2T3[ddd];
        if (!m2t3)
            continue;
        Byte                op = 0x06 | (ddd << 3);
        cpu->m1t4[op] = cpu->state_2bops;
        cpu->m2t3[op] = i8080_state_mviM2T3[ddd];
    }
}

// The IMPL_M2T3(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M2T3(R)                                            \
    static void NAME_M2T3(R) (i8080 cpu, int phase) {           \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            edge_lo(cpu->WAIT);                                 \
            edge_hi(cpu->RETM1_INT);                            \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->R, cpu->DATA->value);                  \
            edge_lo(cpu->DBIN);                                 \
            addr_z(cpu->ADDR);                                  \
            break;                                              \
        case PHI2_FALL:                                         \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M2T3(R)

IMPL_M2T3(A);
IMPL_M2T3(B);
IMPL_M2T3(C);
IMPL_M2T3(D);
IMPL_M2T3(E);
IMPL_M2T3(H);
IMPL_M2T3(L);
