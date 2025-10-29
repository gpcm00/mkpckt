#!/bin/bash

cd $(realpath $(dirname $0))

gcc phex.c -o phex
