#!/bin/bash -

set -euo pipefail
# set -x

rp=$(realpath "$0")
bin=$(dirname "$rp")
mach="$bin"/$MACH/
top=$(dirname "$bin")
dev="$top/bdev"
hex="$top/hex"
scr="$top/scr"
bdev="$top/bdev"

export BDEV_DIR="$dev"

for sf in "$scr"/*.scr
do
    echo loading "$sf" ...
    "$mach/bdev-ld-scr" 0 < "$sf"
done

"$mach/bdev-st" 0 boot-forth
