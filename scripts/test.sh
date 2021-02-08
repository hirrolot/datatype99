#!/bin/bash

mkdir -p tests/build
cd tests/build
cmake ..
cmake --build .
