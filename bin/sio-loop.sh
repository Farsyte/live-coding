#!/bin/bash -
set -euo pipefail
# set -x

self=$(realpath "$0")
bin=$(dirname "$self")

port="$1"

while true
do
    stty raw -echo
    "$bin/sio" "$port"
    stty sane
    sleep 3
done
