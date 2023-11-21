#include "chip/i8080_impl.h"

// i8080_fetch: manage the STATUS_FETCH machine cycle
// T1 drives PC to IDAL to address, STATUS_FETCH to data, and SYNC high.
// T2 drives IDAL+1 to PC, releases Data and Sync, drives DBIN
// TW waits for READY if needed
// T3 samples Data to IR, releases DBIN
// control delivered via cpu->m1t4[IR] for the T4 cycle

static void         maybe_debug(i8080 cpu);

static f8080State   i8080_state_pc_out_status;
static f8080State   i8080_state_pc_inc;
static f8080State   i8080_state_fetch_wait;
static f8080State   i8080_state_op_rd;
static f8080State   i8080_state_nop;

// Initialize the FETCH facility.
//
// Fill in cpu->state_fetch with the function to use to
// manage the STATE_FETCH cycles, and establish the service
// function for the NOP instruction.
//
// This should be the only symbol exported by this file into the
// global namespace.

void i8080_fetch_init(i8080 cpu)
{
    cpu->state_fetch = i8080_state_pc_out_status;

    cpu->m1t4[OP_NOP] = i8080_state_nop;

    // route the decode for all "undefined" instructions
    // to the NOP handler.

    cpu->m1t4[OP__08] = i8080_state_nop;
    cpu->m1t4[OP__10] = i8080_state_nop;
    cpu->m1t4[OP__18] = i8080_state_nop;
    cpu->m1t4[OP__20] = i8080_state_nop;
    cpu->m1t4[OP__28] = i8080_state_nop;
    cpu->m1t4[OP__30] = i8080_state_nop;
    cpu->m1t4[OP__38] = i8080_state_nop;
    cpu->m1t4[OP__CB] = i8080_state_nop;
    cpu->m1t4[OP__D9] = i8080_state_nop;
    cpu->m1t4[OP__DD] = i8080_state_nop;
    cpu->m1t4[OP__ED] = i8080_state_nop;
    cpu->m1t4[OP__FD] = i8080_state_nop;
}

// i8080_state_pc_out_status: publish PC via IDAL, STATUS. Start SYNC.

static void i8080_state_pc_out_status(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          // Messing with IR here breaks ALU writeback in M1T2.
          // DTRI(IR);
          break;
      case PHI2_RISE:
          ASET(IDAL, VAL(PC));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_FETCH);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_pc_inc;
          break;
    }
}

// i8080_state_pc_inc: inc IDAL into PC, end SYNC, start DBIN, check READY

static void i8080_state_pc_inc(i8080 cpu, int phase)
{
    Byte                IR;
    p8080State          m1t2;
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          IR = VAL(IR);
          m1t2 = cpu->m1t2[IR];
          if (NULL != m1t2)
              m1t2(cpu, phase);
          ASET(PC, INC(IDAL));
          LOWER(SYNC);
          DTRI(DATA);
          RAISE(DBIN);
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = i8080_state_op_rd;      // no bist coverage
          else
              cpu->state_next = i8080_state_fetch_wait;
          break;
    }
}

// i8080_state_fetch_wait: raise WAIT, re-raise DBIN, check READY

static void i8080_state_fetch_wait(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(WAIT);
          break;
      case PHI2_RISE:
          // do not issue a falling edge,
          // but do re-issue the rising edge.
          VAL(DBIN) = 0;
          RAISE(DBIN);
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = i8080_state_op_rd;
          break;
    }
}

// i8080_state_op_rd: release WAIT, sample DATA, release DBIN, decode.

static void i8080_state_op_rd(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(IR, VAL(DATA));
          LOWER(DBIN);
          maybe_debug(cpu);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t4[VAL(IR)];
          break;
    }
}

// i8080_state_nop: raise RETM1_INT and do nothing else.

static void i8080_state_nop(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          break;
    }
}

static FILE        *trace;
static unsigned     debug_limit = 1000000;

static void maybe_debug(i8080 cpu)
{

//    pAddr               PC = cpu->PC;
//    pAddr               SP = cpu->SP;
//
//    pData               B = cpu->B;
//    pData               C = cpu->C;
//    pData               D = cpu->D;
//    pData               E = cpu->E;
//    pData               H = cpu->H;
//    pData               L = cpu->L;
//
//    pData               A = cpu->A;
//    pData               FLAGS = cpu->FLAGS;

    Word                PC = cpu->PC->value - 1;
    if ((pc < 0x4000) || (pc >= 0xC000))
        return;

    if (!debug_limit)
        return;
    --debug_limit;
    if (!debug_limit) {
        if (trace) {
            fclose(trace);
            trace = NULL;
        }
        return;
    }

    if (!trace) {
        trace = fopen("trace_fetch", "w");
        if (!trace) {
            STUB("not writing trace file");
            debug_limit = 0;
            return;
        }
    }

    Byte                op = cpu->IR->value;

    Cstr                as = i8080_instruction_4asm(op);

    fprintf(trace, "PC=%04X ", PC);
    fprintf(trace, "%02X ", op);

    if (as[0] == ' ')
        fprintf(trace, "%s", as);
    else if (as[4] == ' ')
        fprintf(trace, "0x__%s", as + 4);
    else
        fprintf(trace, "0x____%s", as + 6);

    fprintf(trace, "SP=%04X ", cpu->SP->value);
    fprintf(trace, "BC=%02X%02X ", cpu->B->value, cpu->C->value);
    fprintf(trace, "DE=%02X%02X ", cpu->D->value, cpu->E->value);
    fprintf(trace, "HL=%02X%02X ", cpu->H->value, cpu->L->value);
    fprintf(trace, "A=%02X ", cpu->A->value);

    fprintf(trace, "FLAGS=%02X%s%s%s%s%s\n",
            cpu->FLAGS->value,
            cpu->FLAGS->value & FLAGS_CY ? " CY" : "",
            cpu->FLAGS->value & FLAGS_P ? " P" : "",
            cpu->FLAGS->value & FLAGS_AC ? " AC" : "",
            cpu->FLAGS->value & FLAGS_Z ? " Z" : "", cpu->FLAGS->value & FLAGS_S ? " S" : "");
    fflush(trace);
}
