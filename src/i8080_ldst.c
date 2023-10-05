#include <stdlib.h>
#include "i8080_impl.h"

#define NAME_LDAX_M1T4(RH,RL)	i8080_state_ldax_M1T4_##RH##RL
#define	DEFN_LDAX_M1T4(RH,RL)	static f8080State NAME_LDAX_M1T4(RH,RL)

#define NAME_LDAX_M2T1(RH,RL)	i8080_state_ldax_M2T1_##RH##RL
#define	DEFN_LDAX_M2T1(RH,RL)	static f8080State NAME_LDAX_M2T1(RH,RL)

#define NAME_LDAX_M2T2(RH,RL)	i8080_state_ldax_M2T2_##RH##RL
#define	DEFN_LDAX_M2T2(RH,RL)	static f8080State NAME_LDAX_M2T2(RH,RL)

#define NAME_LDAX_M2TW(RH,RL)	i8080_state_ldax_M2TW_##RH##RL
#define	DEFN_LDAX_M2TW(RH,RL)	static f8080State NAME_LDAX_M2TW(RH,RL)

#define NAME_LDAX_M2T3(RH,RL)	i8080_state_ldax_M2T3_##RH##RL
#define	DEFN_LDAX_M2T3(RH,RL)	static f8080State NAME_LDAX_M2T3(RH,RL)

#define DEFN_LDAX(RH,RL)   \
    DEFN_LDAX_M1T4(RH,RL); \
    DEFN_LDAX_M2T1(RH,RL); \
    DEFN_LDAX_M2T2(RH,RL); \
    DEFN_LDAX_M2TW(RH,RL); \
    DEFN_LDAX_M2T3(RH,RL)

DEFN_LDAX(B, C);
DEFN_LDAX(D, E);

#define NAME_STAX_M1T4(RH,RL)	i8080_state_stax_M1T4_##RH##RL
#define	DEFN_STAX_M1T4(RH,RL)	static f8080State NAME_STAX_M1T4(RH,RL)

#define NAME_STAX_M2T1(RH,RL)	i8080_state_stax_M2T1_##RH##RL
#define	DEFN_STAX_M2T1(RH,RL)	static f8080State NAME_STAX_M2T1(RH,RL)

#define NAME_STAX_M2T2(RH,RL)	i8080_state_stax_M2T2_##RH##RL
#define	DEFN_STAX_M2T2(RH,RL)	static f8080State NAME_STAX_M2T2(RH,RL)

#define NAME_STAX_M2T3(RH,RL)	i8080_state_stax_M2T3_##RH##RL
#define	DEFN_STAX_M2T3(RH,RL)	static f8080State NAME_STAX_M2T3(RH,RL)

#define DEFN_STAX(RH,RL)   \
    DEFN_STAX_M1T4(RH,RL); \
    DEFN_STAX_M2T1(RH,RL); \
    DEFN_STAX_M2T2(RH,RL); \
    DEFN_STAX_M2T3(RH,RL)

DEFN_STAX(B, C);
DEFN_STAX(D, E);

static f8080State   i8080_state_lda_M2T3;
static f8080State   i8080_state_lda_M3T3;
static f8080State   i8080_state_lda_M4T1;
static f8080State   i8080_state_lda_M4T2;
static f8080State   i8080_state_lda_M4TW;
static f8080State   i8080_state_lda_M4T3;

static f8080State   i8080_state_sta_M2T3;
static f8080State   i8080_state_sta_M3T3;
static f8080State   i8080_state_sta_M4T1;
static f8080State   i8080_state_sta_M4T2;
static f8080State   i8080_state_sta_M4T3;

static f8080State   i8080_state_lhld_M2T3;
static f8080State   i8080_state_lhld_M3T3;
static f8080State   i8080_state_lhld_M4T1;
static f8080State   i8080_state_lhld_M4T2;
static f8080State   i8080_state_lhld_M4TW;
static f8080State   i8080_state_lhld_M4T3;

static f8080State   i8080_state_lhld_M5T1;
static f8080State   i8080_state_lhld_M5T2;
static f8080State   i8080_state_lhld_M5TW;
static f8080State   i8080_state_lhld_M5T3;

static f8080State   i8080_state_shld_M2T3;
static f8080State   i8080_state_shld_M3T3;

static f8080State   i8080_state_shld_M4T1;
static f8080State   i8080_state_shld_M4T2;
static f8080State   i8080_state_shld_M4T3;

static f8080State   i8080_state_shld_M5T1;
static f8080State   i8080_state_shld_M5T2;
static f8080State   i8080_state_shld_M5T3;

// TODO write the function comment.

void i8080_ldst_init(i8080 cpu)
{
    cpu->m1t4[OP_LDAX_BC] = NAME_LDAX_M1T4(B, C);
    cpu->m1t4[OP_LDAX_DE] = NAME_LDAX_M1T4(D, E);

    cpu->m1t4[OP_STAX_BC] = NAME_STAX_M1T4(B, C);
    cpu->m1t4[OP_STAX_DE] = NAME_STAX_M1T4(D, E);

    cpu->m1t4[OP_LDA] = cpu->state_2bops;
    cpu->m1t4[OP_STA] = cpu->state_2bops;

    cpu->m2t3[OP_LDA] = i8080_state_lda_M2T3;
    cpu->m2t3[OP_STA] = i8080_state_sta_M2T3;

    cpu->m3t3[OP_LDA] = i8080_state_lda_M3T3;
    cpu->m3t3[OP_STA] = i8080_state_sta_M3T3;

    cpu->m1t4[OP_LHLD] = cpu->state_2bops;
    cpu->m1t4[OP_SHLD] = cpu->state_2bops;

    cpu->m2t3[OP_LHLD] = i8080_state_lhld_M2T3;
    cpu->m2t3[OP_SHLD] = i8080_state_shld_M2T3;

    cpu->m3t3[OP_LHLD] = i8080_state_lhld_M3T3;
    cpu->m3t3[OP_SHLD] = i8080_state_shld_M3T3;
}

// TODO add comments to IMPL macros below

#define DECL_LDAX_M1T4(RH,RL)	void NAME_LDAX_M1T4(RH,RL) (i8080 cpu, int phase)
#define DECL_LDAX_M2T1(RH,RL)	void NAME_LDAX_M2T1(RH,RL) (i8080 cpu, int phase)
#define DECL_LDAX_M2T2(RH,RL)	void NAME_LDAX_M2T2(RH,RL) (i8080 cpu, int phase)
#define DECL_LDAX_M2TW(RH,RL)	void NAME_LDAX_M2TW(RH,RL) (i8080 cpu, int phase)
#define DECL_LDAX_M2T3(RH,RL)	void NAME_LDAX_M2T3(RH,RL) (i8080 cpu, int phase)

#define	IMPL_LDAX_M1T4(RH,RL)	static DECL_LDAX_M1T4(RH,RL) {          \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_LDAX_M2T1(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_LDAX_M1T4(RH,RL)

#define	IMPL_LDAX_M2T1(RH,RL)	static DECL_LDAX_M2T1(RH,RL) {          \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            ASET(IDAL, (VAL(RH) << 8) | VAL(RL));                       \
            ASET(ADDR, VAL(IDAL));                                      \
            DSET(DATA, STATUS_MREAD);                                   \
            RAISE(SYNC);                                                \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_LDAX_M2T2(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_LDAX_M2T1(RH,RL)

#define	IMPL_LDAX_M2T2(RH,RL)	static DECL_LDAX_M2T2(RH,RL) {          \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            LOWER(SYNC);                                                \
            DTRI(DATA);                                                 \
            RAISE(DBIN);                                                \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if (VAL(READY))                                             \
                cpu->state_next = NAME_LDAX_M2T3(RH,RL);                \
            else                                                        \
                cpu->state_next = NAME_LDAX_M2TW(RH,RL);                \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_LDAX_M2T2(RH,RL)

#define	IMPL_LDAX_M2TW(RH,RL)	static DECL_LDAX_M2TW(RH,RL) {          \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            RAISE(WAIT);                                                \
            break;                                                      \
        case PHI2_RISE:                                                 \
            /* do not issue a falling edge, */                          \
            /* but do re-issue the rising edge. */                      \
            VAL(DBIN) = 0;                                              \
            RAISE(DBIN);                                                \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if (VAL(READY))                                             \
                cpu->state_next = NAME_LDAX_M2T3(RH,RL);                \
            else                                                        \
                cpu->state_next = NAME_LDAX_M2TW(RH,RL);                \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_LDAX_M2TW(RH,RL)

#define	IMPL_LDAX_M2T3(RH,RL)	static DECL_LDAX_M2T3(RH,RL) {          \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            LOWER(WAIT);                                                \
            RAISE(RETM1_INT);                                           \
            break;                                                      \
        case PHI2_RISE:                                                 \
            DSET(A, VAL(DATA));                                         \
            LOWER(DBIN);                                                \
            ATRI(ADDR);                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_LDAX_M2T3(RH,RL)

#define IMPL_LDAX(RH,RL) \
    IMPL_LDAX_M1T4(RH,RL); \
    IMPL_LDAX_M2T1(RH,RL); \
    IMPL_LDAX_M2T2(RH,RL); \
    IMPL_LDAX_M2TW(RH,RL); \
    IMPL_LDAX_M2T3(RH,RL)

IMPL_LDAX(B, C);
IMPL_LDAX(D, E);

#define DECL_STAX_M1T4(RH,RL)	void NAME_STAX_M1T4(RH,RL) (i8080 cpu, int phase)
#define DECL_STAX_M2T1(RH,RL)	void NAME_STAX_M2T1(RH,RL) (i8080 cpu, int phase)
#define DECL_STAX_M2T2(RH,RL)	void NAME_STAX_M2T2(RH,RL) (i8080 cpu, int phase)
#define DECL_STAX_M2T3(RH,RL)	void NAME_STAX_M2T3(RH,RL) (i8080 cpu, int phase)

#define	IMPL_STAX_M1T4(RH,RL)	static DECL_STAX_M1T4(RH,RL) {          \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_STAX_M2T1(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_STAX_M1T4(RH,RL)

#define	IMPL_STAX_M2T1(RH,RL)	static DECL_STAX_M2T1(RH,RL) {          \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            ASET(IDAL, (VAL(RH) << 8) | VAL(RL));                       \
            ASET(ADDR, VAL(IDAL));                                      \
            DSET(DATA, STATUS_MWRITE);                                  \
            RAISE(SYNC);                                                \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_STAX_M2T2(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_STAX_M2T1(RH,RL)

#define	IMPL_STAX_M2T2(RH,RL)	static DECL_STAX_M2T2(RH,RL) {          \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            LOWER(SYNC);                                                \
            DSET(DATA, VAL(A));                                         \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_STAX_M2T3(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_STAX_M2T2(RH,RL)

#define	IMPL_STAX_M2T3(RH,RL)	static DECL_STAX_M2T3(RH,RL) {          \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            RAISE(RETM1_INT);                                           \
            LOWER(WR_);                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            RAISE(WR_);                                                 \
            ATRI(ADDR);                                                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_STAX_M2T3(RH, RL)

#define IMPL_STAX(RH,RL)   \
    IMPL_STAX_M1T4(RH,RL); \
    IMPL_STAX_M2T1(RH,RL); \
    IMPL_STAX_M2T2(RH,RL); \
    IMPL_STAX_M2T3(RH,RL)
IMPL_STAX(B, C);
IMPL_STAX(D, E);

static void i8080_state_lda_M2T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(Z, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->state_3bops_t1;
          break;
    }
}

static void i8080_state_lda_M3T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(W, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_lda_M4T1;
          break;
    }
}

static void i8080_state_lda_M4T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, (VAL(W) << 8) | VAL(Z));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_MREAD);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_lda_M4T2;
          break;
    }
}

static void i8080_state_lda_M4T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DTRI(DATA);
          RAISE(DBIN);
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = i8080_state_lda_M4T3;
          else
              cpu->state_next = i8080_state_lda_M4TW;
          break;
    }
}

static void i8080_state_lda_M4TW(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          // do not issue a falling edge,
          // but do re-issue the rising edge.
          VAL(DBIN) = 0;
          RAISE(DBIN);
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = i8080_state_lda_M4T3;
          else
              cpu->state_next = i8080_state_lda_M4TW;
          break;
    }
}

static void i8080_state_lda_M4T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(A, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_sta_M2T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(Z, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->state_3bops_t1;
          break;
    }
}

static void i8080_state_sta_M3T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(W, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_sta_M4T1;
          break;
    }
}

static void i8080_state_sta_M4T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, (VAL(W) << 8) | VAL(Z));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_MWRITE);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_sta_M4T2;
          break;
    }
}

static void i8080_state_sta_M4T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DSET(DATA, VAL(A));
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_sta_M4T3;
          break;
    }
}

static void i8080_state_sta_M4T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          LOWER(WR_);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          RAISE(WR_);
          ATRI(ADDR);
          break;
    }
}

static void i8080_state_lhld_M2T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(Z, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->state_3bops_t1;
          break;
    }
}

static void i8080_state_lhld_M3T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(W, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_lhld_M4T1;
          break;
    }
}

static void i8080_state_lhld_M4T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, (VAL(W) << 8) | VAL(Z));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_MREAD);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_lhld_M4T2;
          break;
    }
}

static void i8080_state_lhld_M4T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DTRI(DATA);
          RAISE(DBIN);
          DSET(Z, INC(IDAL));
          DSET(W, INC(IDAL) >> 8);
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = i8080_state_lhld_M4T3;
          else
              cpu->state_next = i8080_state_lhld_M4TW;
          break;
    }
}

static void i8080_state_lhld_M4TW(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_lhld_M4T3;
          else
              cpu->state_next = i8080_state_lhld_M4TW;
          break;
    }
}

static void i8080_state_lhld_M4T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(L, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_lhld_M5T1;
          break;
    }
}

static void i8080_state_lhld_M5T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, (VAL(W) << 8) | VAL(Z));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_MREAD);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_lhld_M5T2;
          break;
    }
}

static void i8080_state_lhld_M5T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DTRI(DATA);
          RAISE(DBIN);
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = i8080_state_lhld_M5T3;
          else
              cpu->state_next = i8080_state_lhld_M5TW;
          break;
    }
}

static void i8080_state_lhld_M5TW(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_lhld_M5T3;
          else
              cpu->state_next = i8080_state_lhld_M5TW;
          break;
    }
}

static void i8080_state_lhld_M5T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(H, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_shld_M2T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(Z, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->state_3bops_t1;
          break;
    }
}

static void i8080_state_shld_M3T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(W, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_shld_M4T1;
          break;
    }
}

static void i8080_state_shld_M4T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, (VAL(W) << 8) | VAL(Z));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_MWRITE);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_shld_M4T2;
          break;
    }
}

static void i8080_state_shld_M4T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DSET(DATA, VAL(L));
          DSET(Z, INC(IDAL));
          DSET(W, INC(IDAL) >> 8);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_shld_M4T3;
          break;
    }
}

static void i8080_state_shld_M4T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WR_);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          RAISE(WR_);
          ATRI(ADDR);
          cpu->state_next = i8080_state_shld_M5T1;
          break;
    }
}

static void i8080_state_shld_M5T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, (VAL(W) << 8) | VAL(Z));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_MWRITE);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_shld_M5T2;
          break;
    }
}

static void i8080_state_shld_M5T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DSET(DATA, VAL(H));
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_shld_M5T3;
          break;
    }
}

static void i8080_state_shld_M5T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          LOWER(WR_);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          RAISE(WR_);
          ATRI(ADDR);
          break;
    }
}
