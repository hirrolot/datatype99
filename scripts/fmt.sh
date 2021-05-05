#!/bin/bash

find tests examples \
    \( -path examples/build -o -path tests/build \) -prune -false -o \
    \( -iname "*.h" \) -or \( -iname "*.c" \) | xargs clang-format -i datatype99.h
