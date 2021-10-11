#!/bin/bash

mkdir -p examples/build
cd examples/build
cmake ..
cmake --build .

run_example() {
    echo "executing ./$1 ..."
    ./$1
}

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    run_example "array_in_variant"
    run_example "ast"
    run_example "binary_tree_malloc"
    run_example "binary_tree"
    run_example "token"

    run_example "derive/metadata"
    run_example "derive/record_metadata"
    run_example "derive/print"
    run_example "derive/command_menu"
fi
