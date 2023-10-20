#!/bin/bash -
set -euo pipefail
# set -x

self=$(realpath "$0")
bin=$(dirname "$self")

xterm \
    -title 'Line Printer' \
    -fn -misc-fixed-medium-r-normal--10-100-75-75-c-60-iso10646-1 \
    -fg '#000000' -bg '#E0FFE0' \
    -geometry 132x66-12-60 \
    -sb \
     -e "$bin/sio-loop.sh" 2503
