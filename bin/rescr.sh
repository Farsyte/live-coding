#!/bin/bash -
# set -euo pipefail
# set -x

while true
do
    bdev-ld-scr -g gloss -i index 0 "$@"
    sort < gloss
    sort < index
    inotifywait -q -e modify "$@"
    sleep 1
done
