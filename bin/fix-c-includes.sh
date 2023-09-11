#!/bin/bash -
# set -euo pipefail
# set -x

rp=$(realpath "$0")
dp=$(dirname "$rp")
top=$(dirname "$dp")
tmp="$top/tmp/.cfix"

for c in "$@"
do
    d=$(dirname "$c")
    b=$(basename "$c" .c)
    o="$tmp.$b.c"

    (
        (
            [ ! -e "$top/inc/$b.h" ] ||
                printf '#include "%s.h"\n' "$b"
            grep '#include <' < $c | sort
            grep '#include "' < $c | sort
            ) | uniq.awk
        echo
        grep -v '#include' < $c
    ) | cat -s > $o

    if cmp $c $o >/dev/null
    then
        rm -f "$o"
    else
        mv "$c" "$c"~
        mv "$o" "$c"
    fi
done
