#pragma once
#include "i8080.h"
#include "i8080_flags.h"
#include "i8080_opcodes.h"
#include "i8080_phases.h"
#include "i8080_status.h"

// i8080_impl: shared between parts of the 8080 implementation
//
// The simulation of the 8080 is divided out to a number of
// separate files, containing somewhat independent sections
// of the module which communicate through the internal state
// of the i8080 structure.

extern void         i8080_reset_init(i8080 cpu);
extern void         i8080_fetch_init(i8080 cpu);
extern void         i8080_2bops_init(i8080 cpu);
extern void         i8080_3bops_init(i8080 cpu);

extern void         i8080_eidihlt_init(i8080 cpu);
extern void         i8080_mvi_init(i8080 cpu);
extern void         i8080_mov_init(i8080 cpu);
extern void         i8080_alu_init(i8080 cpu);
extern void         i8080_incdec_init(i8080 cpu);
extern void         i8080_misc_init(i8080 cpu);
extern void         i8080_lxi_init(i8080 cpu);
extern void         i8080_jmp_init(i8080 cpu);
extern void         i8080_io_init(i8080 cpu);
extern void         i8080_movm_init(i8080 cpu);
extern void         i8080_dad_init(i8080 cpu);
extern void         i8080_stack_init(i8080 cpu);

// === === === === === === === === === === === === === === === ===
// Macros to reduce small-scale boilerplate
// === === === === === === === === === === === === === === === ===

// VAL(r) expands to the value of the "r" field in the cpu.
// This can be used on the left side of an assignment,
// but only do so when EXPLICITLY avoiding callback processing.
#define	VAL(x)		(cpu->x->value)

// INC(r) expands to the value of the "r" field in the cpu, plus one.
#define INC(x)		(VAL(x)+1)

// INC(r) expands to the value of the "r" field in the cpu, minus one.
#define DEC(x)		(VAL(x)-1)

// RP(rh,rl) expands to the value of the rh/rl register pair.
#define RP(rh,rl)	PAIR(VAL(rh),VAL(rl))

// PAIR(vh,vl) combines the vh and vl bytes into a word value.
#define PAIR(vh,vl)	(((vh)<<8)|(vl))

// LOWER(x) makes the CPU's "x" signal low, with callback service.
#define LOWER(x)	edge_lo(cpu->x)

// RAISE(x) makes the CPU's "x" signal high, with callback service.
#define RAISE(x)	edge_hi(cpu->x)

// DSET(d,v) sets the CPU's "d" data latch to v, with callback service.
// all of the 8 bit registers are included in the term "latch" here.
#define DSET(d,v)	data_to(cpu->d,v)

// ASET(a,v) sets the CPU's "a" address latch to v, with callback service.
// all of the 16 bit registers are included in the term "latch" here.
#define ASET(a,v)	addr_to(cpu->a,v)

// DTRI(d) marks the CPU's "d" data latch as tristate, with callback service.
// all of the 8 bit registers are included in the term "latch" here.
#define DTRI(d)		data_z(cpu->d)

// ATRI(a) marks the CPU's "a" address latch as tristate, with callback service.
// all of the 16 bit registers are included in the term "latch" here.
#define ATRI(a)		addr_z(cpu->a)
