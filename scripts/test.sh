#!/bin/bash

gcc tests.c -I. -Imetalang99/include -ftrack-macro-expansion=0 -Wall -Wextra -pedantic -std=c99 -o tests
./tests
rm tests
