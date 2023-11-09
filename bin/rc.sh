#!/bin/bash -
# set -euo pipefail
# set -x

# unset DISPLAY
# . ~/.bashrc

self=$(realpath "$0")
bin=$(dirname "$self")
mach="$bin/$MACH"
top=$(dirname "$bin")

export PATH="$mach:$bin:$PATH"

cd "$top"
exec /bin/bash
