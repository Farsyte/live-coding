#!/bin/bash -
set -euo pipefail
set -x

while true
do
    bdev-ld-scr 0 "$@"
    inotifywait -q -e modify "$@"
    sleep 1
done
