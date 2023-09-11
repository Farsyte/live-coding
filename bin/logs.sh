#!/bin/bash -
set -euo pipefail

logdir="$1"

find "$logdir" -empty                                           \
    -o -iname cc-*.stderr                                       \
    -exec printf '\n%s:\n\n' \{\} \;                            \
    -exec cat \{\} \;

find "$logdir" -empty                                           \
    -o -iname run-*.stderr                                      \
    -exec printf '\n%s:\n\n' \{\} \;                            \
    -exec cat \{\} \;

find "$logdir" -empty                                           \
    -o -iname *.log.difference                                  \
    -exec printf '\n%s:\n\n' \{\} \;                            \
    -exec cat \{\} \;
