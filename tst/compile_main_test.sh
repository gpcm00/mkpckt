#!/bin/bash

cd $(realpath $(dirname $0))

rm -f ./raw_bytes.c

../mkpckt -e -f ./packet.txt | ./phex > ./raw_bytes.c

gcc main_test.c raw_bytes.c -o test_packet