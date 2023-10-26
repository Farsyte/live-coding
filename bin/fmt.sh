#!/bin/bash -

set -euo pipefail
# set -x

rp=$(realpath "$0")
bin=$(dirname "$rp")
top=$(dirname "$bin")
dev="$top/bdev"
hex="$top/hex"

export BDEV_DIR="$dev"

# Drive A: boot CP/M
#    VoidStar8080_boot_cpm.hex
#	0080h -> Track 00, Sector 01
#	00FFh -> Track 00, Sector 01
#    cpm22ccp62k.hex
#	DC00h -> Track 00, Sector 02
#	E3B9h -> Track 00, Sector 17
#    cpm22bdos62k.hex
#	E400h -> Track 00, Sector 18
#	F1B2h -> Track 01, Sector 19
#    VoidStar8080_cpm_cbios.hex
#	F200h -> Track 01, Sector 20
#	F49Fh -> Track 01, Sector 25

# Drive A: boot FORTH
#    VoidStar8080_boot_forth.hex
#	0080h -> Track 00, Sector 01
#	00FFh -> Track 00, Sector 01
#    VoidStar8080_forth.hex
#	0100h -> Track 00, Sector 02
#	1A6Fh -> Track 01, Sector 26

# Drive C: mass storage (not bootable) ~2 MiB
# Drive D: mass storage (not bootable) ~8 MiB

"$bin/bdev-format" \
    0 \
    128 26 77 \
    "$hex/VoidStar8080_boot_cpm.hex" \
    "$hex/cpm22ccp62k.hex" \
    "$hex/cpm22bdos62k.hex" \
    "$hex/VoidStar8080_cpm_cbios.hex"

"$bin/bdev-format" \
    1 \
    128 26 77 \
    "$hex/VoidStar8080_boot_forth.hex" \
    "$hex/VoidStar8080_forth.hex"

"$bin/bdev-format" \
    2 \
    128 255 256

"$bin/bdev-format" \
    3 \
    128 255 256
