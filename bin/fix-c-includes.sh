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
            cd "$top"
            find inc -iname "$b.h" -print |
                sed 's:^inc/:#include ":' |
                sed 's:$:":'
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
