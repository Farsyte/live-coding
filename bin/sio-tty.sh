#/bin/bash -
set -euo pipefail
# set -x

self=$(realpath "$0")
bin=$(dirname "$self")

# this looks OK but want something less thin
#     -fn -misc-fixed-medium-r-normal--15-140-75-75-c-90-iso10646-1

xterm \
     -title 'Printing Terminal' \
     -fn -misc-fixed-bold-r-normal--15-140-75-75-c-90-iso10646-1 \
     -bg '#FFEF80' -fg '#000000' \
     -geometry 80x48+3440-60 \
     -sb \
     -e "$bin/sio-loop.sh" 2501

