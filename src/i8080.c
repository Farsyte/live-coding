#include "i8080.h"
#include "clock.h"
#include "decoder.h"
#include "i8080_impl.h"
#include "i8224.h"
#include "i8228.h"
#include "ram8107x8x4.h"
#include "rom8316.h"
#include "sigtrace.h"
#include "target.h"

static void         i8080_phi1_rise(i8080);
static void         i8080_phi2_rise(i8080);
static void         i8080_phi2_fall(i8080);

static f8080State   i8080_state_poweron;

// i8080: Single Chip 8-bit Microprocessor
//
// This module simulates the behavior of the 8080 CPU, matching
// the external signals (within simulation quantization) and modeling
// some but not all known internal state.

// i8080_invar: verify the invariants for a i8080.

void i8080_invar(i8080 cpu)
{
    assert(cpu);
    assert(cpu->name);
    assert(cpu->name[0]);

    addr_invar(cpu->ADDR);
    data_invar(cpu->DATA);

    addr_invar(cpu->PC);
    addr_invar(cpu->SP);
    addr_invar(cpu->IDAL);

    data_invar(cpu->IR);
    data_invar(cpu->TMP);
    data_invar(cpu->ACT);
    data_invar(cpu->ALU);
    data_invar(cpu->FLAGS);

    edge_invar(cpu->PHI1);
    edge_invar(cpu->PHI2);
    edge_invar(cpu->SYNC);
    edge_invar(cpu->DBIN);
    edge_invar(cpu->WR_);
    edge_invar(cpu->READY);
    edge_invar(cpu->WAIT);
    edge_invar(cpu->HOLD);
    edge_invar(cpu->HLDA);
    edge_invar(cpu->INT);
    edge_invar(cpu->INTE);
    edge_invar(cpu->RESET);
}

// i8080_init(s): initialize the given i8080 to have this name
// and an initial state.

void i8080_init(i8080 cpu, Cstr name)
{
    assert(cpu);
    assert(name);
    assert(name[0]);

    pAddr               ADDR = cpu->ADDR;
    pData               DATA = cpu->DATA;

    pEdge               SYNC = cpu->SYNC;
    pEdge               DBIN = cpu->DBIN;
    pEdge               WR_ = cpu->WR_;

    pEdge               WAIT = cpu->WAIT;
    pEdge               HOLD = cpu->HOLD;
    pEdge               HLDA = cpu->HLDA;

    pEdge               INT = cpu->INT;
    pEdge               INTE = cpu->INTE;

    pAddr               PC = cpu->PC;
    pAddr               SP = cpu->SP;
    pAddr               IDAL = cpu->IDAL;

    pData               A = cpu->A;

    pData               B = cpu->B;
    pData               C = cpu->C;
    pData               D = cpu->D;
    pData               E = cpu->E;
    pData               H = cpu->H;
    pData               L = cpu->L;

    pData               IR = cpu->IR;
    pData               TMP = cpu->TMP;
    pData               ACT = cpu->ACT;
    pData               ALU = cpu->ALU;
    pData               FLAGS = cpu->FLAGS;

    pEdge               RESET_INT = cpu->RESET_INT;
    pEdge               RETM1_INT = cpu->RETM1_INT;
    pEdge               INH_PC_INC = cpu->INH_PC_INC;

    cpu->name = name;

    addr_init(ADDR, format("%s:ADDR", name));
    data_init(DATA, format("%s:DATA", name));

    data_init(DATA, format("%s:DATA", name));

    edge_init(SYNC, format("%s:SYNC", name), 0);
    edge_init(DBIN, format("%s:DBIN", name), 0);
    edge_init(WR_, format("%s:/WR", name), 1);
    edge_init(WAIT, format("%s:WAIT", name), 0);
    edge_init(HOLD, format("%s:HOLD", name), 0);
    edge_init(HLDA, format("%s:HLDA", name), 0);
    edge_init(INT, format("%s:INT", name), 0);
    edge_init(INTE, format("%s:INTE", name), 0);

    addr_init(PC, format("%s:PC", name));
    addr_init(SP, format("%s:SP", name));
    addr_init(IDAL, format("%s:IDAL", name));

    data_init(A, format("%s:A", name));

    data_init(B, format("%s:B", name));
    data_init(C, format("%s:C", name));
    data_init(D, format("%s:D", name));
    data_init(E, format("%s:E", name));
    data_init(H, format("%s:H", name));
    data_init(L, format("%s:L", name));

    data_init(IR, format("%s:IR", name));
    data_init(TMP, format("%s:TMP", name));
    data_init(ACT, format("%s:ACT", name));
    data_init(ALU, format("%s:ALU", name));
    data_init(FLAGS, format("%s:FLAGS", name));

    edge_init(RESET_INT, format("%s:RESET_INT", name), 1);
    edge_init(RETM1_INT, format("%s:RETM1_INT", name), 0);
    edge_init(INH_PC_INC, format("%s:INH_PC_INC", name), 0);

    cpu->state_reset = i8080_state_poweron;
    cpu->state_fetch = i8080_state_poweron;
    cpu->state_2bops = i8080_state_poweron;
    cpu->state_2bops_t1 = i8080_state_poweron;
    cpu->state_3bops_t1 = i8080_state_poweron;

    cpu->state = i8080_state_poweron;
    cpu->state_next = i8080_state_poweron;
    cpu->state_m1t1 = i8080_state_poweron;

    for (unsigned inst = 0x00; inst <= 0xFF; inst++)
        cpu->m1t2[inst] = NULL;
    for (unsigned inst = 0x00; inst <= 0xFF; inst++)
        cpu->m1t4[inst] = i8080_state_poweron;
    for (unsigned inst = 0x00; inst <= 0xFF; inst++)
        cpu->m1t5[inst] = i8080_state_poweron;
    for (unsigned inst = 0x00; inst <= 0xFF; inst++)
        cpu->m2t3[inst] = i8080_state_poweron;
    for (unsigned inst = 0x00; inst <= 0xFF; inst++)
        cpu->m3t3[inst] = i8080_state_poweron;
}

// I8080_INIT(s): initialize the given i8080 to have this name
// and an initial state.

#define I8080_INIT(gen) i8080_init(gen, #gen)

// i8080_linked(s): finish initialization, called after pEdge fields are set.

void i8080_linked(i8080 cpu)
{
    i8080_invar(cpu);

    ASSERT_EQ_integer(0, cpu->READY->value);
    ASSERT_EQ_integer(1, cpu->RESET->value);
    ASSERT_EQ_integer(1, cpu->RESET_INT->value);
    ASSERT_EQ_integer(0, cpu->RETM1_INT->value);
    ASSERT_EQ_integer(0, cpu->INH_PC_INC->value);

    i8080_reset_init(cpu);
    i8080_fetch_init(cpu);
    i8080_2bops_init(cpu);
    i8080_3bops_init(cpu);
    i8080_eidihlt_init(cpu);
    i8080_mvi_init(cpu);
    i8080_mov_init(cpu);
    i8080_alu_init(cpu);
    i8080_incdec_init(cpu);
    i8080_misc_init(cpu);
    i8080_lxi_init(cpu);
    i8080_dad_init(cpu);

    EDGE_ON_RISE(cpu->PHI1, i8080_phi1_rise, cpu);
    EDGE_ON_RISE(cpu->PHI2, i8080_phi2_rise, cpu);
    EDGE_ON_FALL(cpu->PHI2, i8080_phi2_fall, cpu);
}

// i8080_unimp_ops(i8080 cpu): count opcodes missing from m1t4 table

unsigned i8080_unimp_ops(i8080 cpu)
{
    unsigned            unset_count = 0;

    FILE               *fp = fopen("log/i8080_unimp.txt", "w");

    for (unsigned inst = 0x00; inst <= 0xFF; inst++) {
        p8080State          m1t4 = cpu->m1t4[inst];
        if (m1t4 != i8080_state_poweron)
            continue;
        fprintf(fp, "%02X %s\n", inst, i8080_instruction_name(inst));
        ++unset_count;
    }
    fclose(fp);
    return unset_count;
}

// i8080_phi1_rise: actions at start of T-state

static void i8080_phi1_rise(i8080 cpu)
{
    if (cpu->RETM1_INT->value) {
        cpu->state_next = cpu->state_m1t1;
        edge_lo(cpu->RETM1_INT);
    }

    cpu->state = cpu->state_next;
    cpu->state(cpu, PHI1_RISE);
}

// i8080_phi2_rise: actions early in T-state

static void i8080_phi2_rise(i8080 cpu)
{
    cpu->state(cpu, PHI2_RISE);
}

// i8080_phi2_rise: actions late in T-state

static void i8080_phi2_fall(i8080 cpu)
{
    cpu->state(cpu, PHI2_FALL);
    // if in last T-state of a M-cycle,
    // then set state_next to state_m1t1.
}

// i8080_state_poweron: T-state control default
//
// Pointers to this method are used to indicate that
// the processor is in a state not yet simulated.
//
// for example, after decoding an opcode for which
// the M1-T4 control function is not yet set up.
//
// NO BIST COVERAGE for ending up in this fatal state

static void i8080_state_poweron(i8080 cpu, int phase)
{
    STUB("cpu '%s' in phase %d", cpu->name, phase);
    STUB("  PC=0x%04X IR=0x%02X", cpu->PC->value, cpu->IR->value);
    abort();
}
