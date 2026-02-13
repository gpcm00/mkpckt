#!/usr/bin/env bash

# Optional:
#   ./compile_spidev_test.sh /dev/spidev1.0
# or
#   ./compile_spidev_test.sh

cd $(realpath $(dirname $0))

CFLAGS=""

if [[ -n "$1" ]]; then
    CFLAGS="-DSPIDEV_PATH=\"$1\""
fi

gcc $CFLAGS spidev_test.c -o spidev_test