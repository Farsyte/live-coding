#!/bin/bash -
# set -euo pipefail
# set -x

rp=$(realpath "$0")
bd=$(dirname "$rp")

while true
do
    clear
    rm -f gloss index
    bdev-ld-scr -g gloss -i index 0 *.scr

#    cat index

    <index awk -f "$bd/dedup-i.awk"

#    <gloss awk -f "$bd/dedup-g.awk"
    inotifywait -q -e modify *.scr
    sleep 1
done
