#!/bin/bash -
# set -euo pipefail
# set -x

rp=$(realpath "$0")
dp=$(dirname "$rp")
top=$(dirname "$dp")

exec make -C "$top" -k -j cycle
