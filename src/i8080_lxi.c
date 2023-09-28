#include "clock.h"
#include "i8080_impl.h"

// i8080_lxi: manage LXI instructions

// This is one way to not write nearly the same function
// over and over.

#define NAME_M2T3(RH,RL)	i8080_state_lxiM2T3 ## RH##RL
#define DEFN_M2T3(RH,RL)	static f8080State NAME_M2T3(RH,RL)

DEFN_M2T3(B, C);
DEFN_M2T3(D, E);
DEFN_M2T3(H, L);
DEFN_M2T3(S, P);

static p8080State   i8080_state_lxiM2T3[8] = {
    NAME_M2T3(B, C),
    NAME_M2T3(D, E),
    NAME_M2T3(H, L),
    NAME_M2T3(S, P),
};

#define NAME_M3T3(RH,RL)	i8080_state_lxiM3T3 ## RH##RL
#define DEFN_M3T3(RH,RL)	static f8080State NAME_M3T3(RH,RL)

DEFN_M3T3(B, C);
DEFN_M3T3(D, E);
DEFN_M3T3(H, L);
DEFN_M3T3(S, P);

static p8080State   i8080_state_lxiM3T3[8] = {
    NAME_M3T3(B, C),
    NAME_M3T3(D, E),
    NAME_M3T3(H, L),
    NAME_M3T3(S, P),
};

// i8080_lxi_init: set up decoding for LXI instructions

void i8080_lxi_init(i8080 cpu)
{
    for (int rp = 0; rp < 4; ++rp) {
        Byte                op = 0x01 | (rp << 4);
        cpu->m1t4[op] = cpu->state_2bops;
        cpu->m2t3[op] = i8080_state_lxiM2T3[rp];
        cpu->m3t3[op] = i8080_state_lxiM3T3[rp];
    }
}

// The IMPL_M2T3(RH,RL) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M2T3(RH,RL)                                        \
    static void NAME_M2T3(RH,RL) (i8080 cpu, int phase) {       \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            edge_lo(cpu->WAIT);                                 \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->RL, cpu->DATA->value);                 \
            edge_lo(cpu->DBIN);                                 \
            addr_z(cpu->ADDR);                                  \
            break;                                              \
        case PHI2_FALL:                                         \
            cpu->state_next = cpu->state_3bops_t1;              \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M2T3(RH,RL)

IMPL_M2T3(B, C);
IMPL_M2T3(D, E);
IMPL_M2T3(H, L);

static void i8080_state_lxiM2T3SP(i8080 cpu, int phase)
{
    unsigned            SP;
    unsigned            P;

    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          break;
      case PHI2_RISE:
          // write cpu->DATA->value to low byte of SP
          P = cpu->DATA->value;
          SP = cpu->SP->value;
          SP = (SP & 0xFF00) | P;
          addr_to(cpu->SP, SP);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->state_3bops_t1;
          break;
    }
}

// The IMPL_M3T3(RH,RL) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M3T3(RH,RL)                                        \
    static void NAME_M3T3(RH,RL) (i8080 cpu, int phase) {       \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            edge_lo(cpu->WAIT);                                 \
            edge_hi(cpu->RETM1_INT);                            \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->RH, cpu->DATA->value);                 \
            edge_lo(cpu->DBIN);                                 \
            addr_z(cpu->ADDR);                                  \
            break;                                              \
        case PHI2_FALL:                                         \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M3T3(RH,RL)

IMPL_M3T3(B, C);
IMPL_M3T3(D, E);
IMPL_M3T3(H, L);

static void i8080_state_lxiM3T3SP(i8080 cpu, int phase)
{
    unsigned            S;
    unsigned            SP;

    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          // write cpu->DATA->value to low byte of SP
          S = cpu->DATA->value;
          SP = cpu->SP->value;
          SP = (S << 8) | (SP & 0x00FF);
          addr_to(cpu->SP, SP);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          break;
    }
}
