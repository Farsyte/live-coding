# This GNUmakefile creates a program called "live-coding"
# which contains at least some C++ code.
PROG		:= live-coding

# Select C compiler and flags
CC		:= gcc

# set PRO to -pg for profiling.
PRO             :=

# set COV to -fprofile-arcs -ftest-coverage
# to enable test coverage via gcov
# (NOT TESTED)
COV		:=

# set DBG to -g for debugging.
DBG             := -g

# set OPT to -O0, -O1, -O2, -O3, or -Ofast
# for increasing performace.
OPT             := -O3

STD		:= --std=gnu99
WFLAGS		:= -W -Wall
# could add  -Wextra
# could add -Wpedantic
# if so, may need -Wno-missing-field-initializers

include GNUmakefile.rules

WAITF		:= ${CSRC} ${HSRC} ${INST}
WAITE		:= modify delete

run::		${PROG:%=log/run-%.log}
	$C ${INST} > log/run-${PROG}.log 2> log/run-${PROG}.stderr

TEST_OBS	:= ${PROG:%=log/run-%.log}
TEST_EXP	:= ${PROG:%=log/run-%.log.expected}
TEST_CMP	:= ${PROG:%=log/run-%.log.difference}

log/run-%.log:	bin/%
	$< > $@ 2>log/run-$*.stderr

log/run-%.log.difference:	log/run-%.log
	$Q bin/check.sh log/run-$*.log.expected $< $@

clean::
	${RF} log/run-*.stderr

# Set up a build rule that I can run in a loop,
# usually with a "make await" between iteraions.

loop::
	$C ${MAKE} cmp

# Set up a "make cmp" rule that always
# runs the program and compares the results.

cmp::
	${RF} ${TEST_OBS} ${TEST_CMP}
	$C ${MAKE} ${TEST_OBS}
	$C ${MAKE} ${TEST_CMP}

# wait until it is time to build again.

wait::
	$C inotifywait -q -e modify -e delete ${HSRC} ${CSRC} GNUmakefile GNUmakefile.rules

# Add a "make cmp" to the bottom of the "make all" list.
all::
	$C ${MAKE} cmp

# Add removal of the output compare logs to the "make clean" list.
clean::
	${RF} ${TEST_OBS} ${TEST_CMP}

# Set up a "make gdb" rule that makes sure
# we are built, and starts GDB.

gdb::		${INST}
	gdb ${INST}

# Set up a "make format" rule that fixes the
# formatting in all source files.

format::	${INST}
	$E 'fix c headers ...'
	$C bin/fix-c-includes.sh ${CSRC} ${BSRC} ${ASRC}
	$E 'fix h headers ...'
	$C bin/fix-h-includes.sh ${HSRC}
	$E 'indent ...'
	$C bin/indent.sh ${HSRC} ${CSRC} ${BSRC} ${ASRC}


world::
	$C $(MAKE) clean
	$C $(MAKE) cmp

logs:
	$C bin/logs.sh log
