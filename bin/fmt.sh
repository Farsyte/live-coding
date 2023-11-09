#!/bin/bash -

set -euo pipefail
# set -x

rp=$(realpath "$0")
bin=$(dirname "$rp")
mach="$bin"/$MACH/
top=$(dirname "$bin")
dev="$top/bdev"
hex="$top/hex"
bdev="$top/bdev"

export BDEV_DIR="$dev"

# Use bdev-fmt to create these drives:
#
# Drive A: boot FORTH, double density
#    VoidStar8080_boot_forth.hex
#	0080h -> Track 00, Sector 01
#	00FFh -> Track 00, Sector 01
#    VoidStar8080_forth.hex
#	0100h -> Track 00, Sector 02
#	1A6Fh -> Track 01, Sector 26
#
# Drive B: boot CP/M, single density
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
#
# Drive C: mass storage (not bootable) ~2 MiB
# Drive D: mass storage (not bootable) ~8 MiB
#
# Store these images:
#   store A into boot-forth
#   store B into boot-cpm
#   store C into big-1
#   store D into big-2

function fmt_forth() {
    "$mach/bdev-fmt" \
        "$1" \
        128 52 77 \
        "$hex/VoidStar8080_boot_forth.hex" \
        "$hex/VoidStar8080_forth.hex"

    "$mach/bdev-st" "$1" boot-forth
}

function fmt_cpm() {
    "$mach/bdev-fmt" \
        "$1" \
        128 26 77 \
        "$hex/VoidStar8080_boot_cpm.hex" \
        "$hex/cpm22ccp62k.hex" \
        "$hex/cpm22bdos62k.hex" \
        "$hex/VoidStar8080_cpm_cbios.hex"
    "$mach/bdev-st" "$1" boot-cpm
}

function fmt_2mb() {
    "$mach/bdev-fmt" \
        "$1" \
        128 128 128
}

function fmt_8mb() {
    "$mach/bdev-fmt" \
        "$1" \
        128 255 256
}

# Create the files backing "currently mounted" volumes
# if they do not already exist.
# - initally constructed to exercise the bdev-fmt program.

[ -e "$bdev/drive-A.mmap" ] ||
    fmt_forth 0
[ -e "$bdev/drive-B.mmap" ] ||
    fmt_cpm 1
[ -e "$bdev/drive-C.mmap" ] ||
    fmt_2mb 2
[ -e "$bdev/drive-D.mmap" ] ||
    fmt_8mb 3

# Create initial snapshots of formatted volumes if the
# snapshots do not exist.
# - initally constructed to exercise the bdev-st program.

[ -e "$bdev/boot-forth.mmap" ] ||
    "$mach/bdev-st" 0 boot-forth
[ -e "$bdev/boot-cpm.mmap" ] ||
    "$mach/bdev-st" 1 boot-cpm
[ -e "$bdev/big-1.mmap" ] ||
    "$mach/bdev-st" 2 big-1
[ -e "$bdev/big-2.mmap" ] ||
    "$mach/bdev-st" 3 big-2

# Mount (copies of) the snapshot files.
# - initally constructed to exercise the bdev-ld program.
# - detects "trying to mount too-large content file"

"$mach/bdev-ld" 0 boot-forth
"$mach/bdev-ld" 1 boot-cpm
"$mach/bdev-ld" 2 big-1
"$mach/bdev-ld" 3 big-2
