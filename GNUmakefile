# This GNUmakefile creates a program called "live-coding"
# which contains at least some C++ code.
PROG		:= live-coding

# Select C compiler and flags
CC		:= gcc

# set PRO to -pg for profiling.
PRO		:=
# PRO		:= -pg

# set COV to --coverage
# to enable test coverage via gcov
# NOTE: test coverage is never going to be 100%
# even if we manage to automatically ignore the
# benchmark code.
COV		:=
# COV		:= --coverage

# set DBG to -g for debugging. including this flag
# does not seem to reduce performance significantly.
DBG		:= -g

# set OPT to -O0, -O1, -O2, -O3, or -Ofast.
# OPT             := -Ofast
OPT		:= -Ofast
# OPT		:= -O0

STD		:= --std=gnu99
WFLAGS		:= -W -Wall -Wextra -Wpedantic

include GNUmakefile.rules

WAITE		:= modify delete

run::		${EXEC}
	$C ${EXEC} > log/run-${PROG}.log 2> log/run-${PROG}.stderr

TEST_OBS	:= ${PROG:%=log/run-%.log}
TEST_EXP	:= ${PROG:%=log/run-%.log.expected}
TEST_CMP	:= ${PROG:%=log/run-%.log.difference}

log/run-%.log:	bin/%
	${RF} ${OBJDIR}*.gcda
	$< bist > $@ 2>log/run-$*.stderr

log/run-%.log.difference:	log/run-%.log
	$Q bin/check.sh log/run-$*.log.expected $< $@

clean::
	${RF} log/run-*.stderr

# Set up a build rule that I can run in a loop,
# usually with a "make await" between iteraions.

loop::
	$C ${MAKE} lsts
	$C ${MAKE} cmp

bench::		log/run-${PROG}.log.difference
	$P 'benchmarking with OPT="%s" DBG="%s" COV="%s"\n' "${OPT}" "${DBG}" "${COV}"
	$C ${EXEC} bench

# Set up a "make cmp" rule that always
# runs the program and compares the results.

cmp::
	${RF} ${TEST_OBS} ${TEST_CMP}
	$C ${MAKE} ${TEST_OBS}
	$C ${MAKE} ${TEST_CMP}

# wait until it is time to build again.

wait::
	$C inotifywait \
		-q ${WAITE:%=-e %} \
		GNUmakefile GNUmakefile.rules \
		${HSRC} ${CSRC} ${TSRC} ${ASMS}

# Add a "make cmp" to the bottom of the "make all" list.
all::
	$C ${MAKE} cmp

# Add removal of the output compare logs to the "make clean" list.
clean::
	${RF} ${TEST_OBS} ${TEST_CMP}

# Set up a "make gdb" rule that makes sure
# we are built, and starts GDB.

gdb::		${EXEC}
	gdb ${EXEC}

# Set up a "make format" rule that fixes the
# formatting in all source files.

format::	${EXEC}
	$E 'fix c headers ...'
	$C bin/fix-c-includes.sh ${CSRC} ${TSRC}
	$E 'fix h headers ...'
	$C bin/fix-h-includes.sh ${HSRC}
	$E 'indent ...'
	$C bin/indent.sh ${HSRC} ${CSRC} ${TSRC}


world::
	$C $(MAKE) clean
	$C $(MAKE) cmp

logs:
	$C bin/logs.sh log

cycle::
	$C $(MAKE) logs
	$C $(MAKE) plots
	$C $(MAKE) bench
	$C $(MAKE) format

# remove the files produced by "timing.c"

clean::
	${RF} log/timing/*.txt
	${RF} log/timing/*/*.log
