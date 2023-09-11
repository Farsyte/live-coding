#!/bin/bash -
# set -euo pipefail
# set -x

rp=$(realpath "$0")
dp=$(dirname "$rp")
top=$(dirname "$dp")
tmp="$top/tmp/.hfix"

for h in "$@"
do
    d=$(dirname "$h")
    b=$(basename "$h")
    o="$tmp.$b.c"

    (
        echo '#pragma once' > $o
        grep '#include' < $h | sort
        grep -v '#pragma once' < $h | grep -v '#include'
    ) | cat -s >> $o

    if cmp $h $o >/dev/null
    then
        rm -f "$o"
    else
        mv "$h" "$h"~
        mv "$o" "$h"
    fi
done
