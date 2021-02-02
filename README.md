# datatype99
[![CI](https://github.com/Hirrolot/datatype99/workflows/C/C++%20CI/badge.svg)](https://github.com/Hirrolot/datatype99/actions)

datatype99 is a header-only library that augments C99 with [sum types]. It looks like this:

[sum types]: https://en.wikipedia.org/wiki/Tagged_union

[ [`examples/binary_tree.c`](examples/binary_tree.c) ]
```c
// Sums all nodes of a binary tree.

#include <datatype99.h>

#include <stdio.h>

datatype(
    BinaryTree,
    (Leaf, int),
    (Node, struct BinaryTree *, int, struct BinaryTree *)
);

int sum(const BinaryTree *tree) {
    match(*tree) {
        of(Leaf, x) {
            return *x;
        }
        of(Node, lhs, x, rhs) {
            return sum(*lhs) + *x + sum(*rhs);
        }
    }
}

#define TREE(tree)                ((BinaryTree *)(BinaryTree[]){tree})
#define NODE(left, number, right) TREE(Node(left, number, right))
#define LEAF(number)              TREE(Leaf(number))

int main(void) {
    const BinaryTree *tree = NODE(NODE(LEAF(1), 2, NODE(LEAF(3), 4, LEAF(5))), 6, LEAF(7));

    /*
     * Output:
     * 28
     */
    printf("%d\n", sum(tree));
}
```

## Table of contents

 - [Highlights](#highlights)
 - [Usage](#usage)

## Highlights

 - **Type-safe.** In contrast to manually implemented tagged unions, datatype99 provides a type-safe interface (you cannot access an invalid data and cannot construct an invalid variant).

 - **Pure C99.** No external tools are required; datatype99 is implemented using only preprocessor macros.

 - **Zero boilerplate.** datatype99 deals with all the dirty stuff.

## Usage
