#!/usr/bin/env bash

# Optional:
#   ./build.sh /dev/spidev1.0
# or
#   ./build.sh

cd $(realpath $(dirname $0))

CFLAGS=""

if [[ -n "$1" ]]; then
    CFLAGS="-DSPIDEV_PATH=\"$1\""
fi

gcc $CFLAGS spidev_test.c -o spidev_test