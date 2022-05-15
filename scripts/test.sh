#!/bin/bash

set -e

mkdir -p tests/build
cd tests/build
cmake ..
cmake --build .

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    echo "Executing ./tests ..."
    ./tests

    echo "Executing ./derive ..."
    ./derive

    echo "Executing ./metalang99_compliant ..."
    ./metalang99_compliant
fi
