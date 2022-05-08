// The binary_tree.c counterpart that allocates & releases trees using malloc & free.

#include <datatype99.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clang-format off
datatype(
    BinaryTree,
    (Leaf, int),
    (Node, BinaryTree *, int, BinaryTree *)
);
// clang-format on

BinaryTree *alloc_tree(BinaryTree tree) {
    BinaryTree *res = malloc(sizeof(*res));
    assert(res);
    memcpy((void *)res, (const void *)&tree, sizeof(tree));
    return res;
}

void destroy_tree(BinaryTree *tree) {
    match(*tree) {
        of(Leaf, _) {
            free(tree);
            return;
        }
        of(Node, lhs, _, rhs) {
            destroy_tree(*lhs);
            destroy_tree(*rhs);
            free(tree);
        }
    }
}

int sum(const BinaryTree *tree) {
    match(*tree) {
        of(Leaf, x) return *x;
        of(Node, lhs, x, rhs) return sum(*lhs) + *x + sum(*rhs);
    }

    // Invalid input (no such variant).
    return -1;
}

#define TREE(tree)                alloc_tree(tree)
#define NODE(left, number, right) TREE(Node(left, number, right))
#define LEAF(number)              TREE(Leaf(number))

int main(void) {
    const BinaryTree *tree = NODE(NODE(LEAF(1), 2, NODE(LEAF(3), 4, LEAF(5))), 6, LEAF(7));

    /*
     * Output:
     * 28
     */
    printf("%d\n", sum(tree));

    return 0;
}
