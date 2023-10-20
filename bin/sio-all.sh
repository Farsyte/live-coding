#!/bin/bash -
set -euo pipefail
# set -x

self=$(realpath "$0")
bin=$(dirname "$self")

killall sio-loop.sh >& /dev/null|| true
sleep 1

"$bin/sio-crt.sh" &
"$bin/sio-lpt.sh" &
"$bin/sio-tty.sh" &
