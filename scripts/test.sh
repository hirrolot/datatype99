#!/bin/bash

mkdir -p tests/build
cd tests/build
cmake ..
cmake --build .

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    echo "Executing ./tests ..."
    ./tests

    echo "Executing ./derive ..."
    ./derive
fi
