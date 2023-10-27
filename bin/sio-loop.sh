#!/bin/bash -
set -euo pipefail
# set -x

self=$(realpath "$0")
bin=$(dirname "$self")
mach="$bin/$MACH"

port="$1"

while true
do
    stty raw -echo
    "$mach/sio" "$port"
    stty sane
    sleep 3
done
