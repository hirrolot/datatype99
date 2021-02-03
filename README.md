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

 - **Can be used everywhere.** Literally everywhere provided that you have a standard-confirming C99 preprocessor. Even on freestanding environments.

## Usage

A sum type is created using the `datatype` macro. I guess you have already caught the syntax but actually there exist one more kind of a variant: an empty variant which is expressed simply as `(Foo)`. It holds no data.

Pattern matching is likewise intuitive. Just two brief notes:

 - To match an empty variant, write `of(Foo) { ... }`.
 - To match the default case, i.e. when all other cases failed, write `otherwise { ... }`.

Happy hacking!

## Syntax and semantics

### EBNF syntax

```ebnf
<datatype>      ::= "datatype99(" <datatype-name> { "," <variant> }+ ")" ;
<variant>       ::= "(" <variant-name> [ { "," <type> }+ ] ")" ;
<datatype-name> ::= <ident> ;
<variant-name>  ::= <ident> ;

<match>         ::= "match99(" <expr> ")" { <arm> }+ ;
<matches>       ::= "matches99(" <expr> "," <ident> ")" ;
<of>            ::= "of99(" <variant-name> [ { "," <ident> }+ ] ")" <statement> ;
<otherwise>     ::= "otherwise99" <statement> ;
```

### Semantics

#### `datatype99`

This macro accepts a sum type name as a first argument and the rest of arguments shall be comma-separated variants.

 1. For each non-empty variant, the following type definition is generated (the metavariable `<type>` ranges over a corresponding variant's types):

```
typedef struct <datatype-name><variant-name> {
    <type>0 _0;
    ...
    <type>N _N;
} <datatype-name><variant-name>;
```

 2. For each non-empty variant, the following type definitions to types of each field of `<datatype-name><variant-name>` are generated:

```
typedef <type>0 <variant-name>_0;
...
typedef <type>N <variant-name>_N;
```

 3. For each variant, the following type definition to a corresponding sum type is generated:

```
typedef struct <datatype-name> <variant-name>SumT;
```

 4. For each sum type, the following tagged union is generated (inside the union, only fields to structures of non-empty variants are generated):

```
typedef struct <datatype-name> {
    enum {
        <variant-name>0Tag, ..., <variant-name>NTag 
    } tag;

    union {
        char dummy;
        <datatype-name><variant-name>0 <variant-name>0;
        ...
        <datatype-name><variant-name>N <variant-name>N;
    } data;
} <datatype-name> ;
```

 5. For each variant, the following function called a _value constructor_ is generated:

```
inline static <datatype99-name> <variant-name>(...) { /* ... */ }
```

### `match99`

This macro implements [pattern matching] for an instance of a sum type. It accepts an expression of a sum type as a single argument. Afterwards, a chain of arms shall follow.

`match99` has the expected semantics: it tries to match the given instance of a sum type with the given variants, and, if a match has succeeded, it executes the corresponding statement and moves down to the next instruction. If all matches have failed, it executes the statement after `otherwise99` and moves down to the next instruction.

### `matches99`

This macro just tests an instance of a sum type for a given variant. If the given instance corresponds to the given variant, it expands to truthfulness, otherwise, it expands to falsehood.

[pattern matching]: https://en.wikipedia.org/wiki/Pattern_matching
