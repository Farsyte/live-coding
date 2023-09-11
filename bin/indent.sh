#!/bin/bash -
set -euo pipefail
# set -x

rp=$(realpath "$0")
dp=$(dirname "$rp")
top=$(dirname "$dp")
tmp="$top/tmp/.indented"

for f in "$@"
do
    b=$(basename "$f")
    o="$tmp.$b"
    indent "$f" -o "$o"
    if cmp "$f" "$o" > /dev/null
    then
        rm -f "$o"
    else
        mv "$f" "$f"~
        mv "$o" "$f"
    fi
done
