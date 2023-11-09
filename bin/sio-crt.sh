#!/bin/bash -
set -euo pipefail
# set -x

self=$(realpath "$0")
bin=$(dirname "$self")

font="-misc-fixed-medium-r-normal--20-200-75-75-c-100-iso10646-1"
bg="#000000" # black
fg="#00FF00" # green

xterm \
     -title 'Video Terminal' \
     -fn "$font" -fg "$fg" -bg "$bg" \
     -geometry 64x16-12-833 \
     +sb \
     -e "$bin/sio-loop.sh" 2502
