#!/usr/bin/env python

ob, cb = '{', '}'

def gen_t4(sss):
    print(f'''\
static void i8080_state_movT4{sss}(i8080 cpu, int phase)
{ob}
    switch (phase) {ob}
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_to(cpu->TMP, cpu->{sss}->value);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t5[cpu->IR->value];
          break;
    {cb}
{cb}
    ''')

gen_t4('B');
gen_t4('C');
gen_t4('D');
gen_t4('E');
gen_t4('H');
gen_t4('L');
gen_t4('A');

def gen_t5(ddd):
    print(f'''\
static void i8080_state_movT5{ddd}(i8080 cpu, int phase)
{ob}
    switch (phase) {ob}
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->{ddd}, cpu->TMP->value);
          break;
      case PHI2_FALL:
          break;
    {cb}
{cb}
    ''')

gen_t5('B');
gen_t5('C');
gen_t5('D');
gen_t5('E');
gen_t5('H');
gen_t5('L');
gen_t5('A');

