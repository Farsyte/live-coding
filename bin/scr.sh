#!/bin/bash -
set -euo pipefail
set -x

rp=$(realpath "$0")
bin=$(dirname "$rp")
top=$(dirname "$bin")

disc="$1"
shift

sub="$1"
shift

bdev-ld-scr \
    -i "$top/scr/$sub/index" \
    -g "$top/scr/$sub/gloss" \
    "$disc" "$top/scr/$sub/"*.scr
