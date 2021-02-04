#!/bin/bash

gcc tests.c -I. -Iepilepsy/include -ftrack-macro-expansion=0 -Wall -Wextra -pedantic -std=c99 -o tests
./tests
rm tests
