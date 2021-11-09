<div align="center">
  <img src="preview.png" width="600"/>
  <h1>Datatype99</h1>
  <a href="https://github.com/Hirrolot/datatype99/actions">
    <img src="https://github.com/Hirrolot/datatype99/workflows/C/C++%20CI/badge.svg">
  </a>
  <a href="https://lists.sr.ht/~hirrolot/metalang99">
    <img src="https://img.shields.io/badge/mailing%20list-lists.sr.ht-orange">
  </a>

  Safe, intuitive [algebraic data types] with exhaustive pattern matching & compile-time introspection facilities. No external tools required, pure C99.
</div>

[algebraic data types]: https://en.wikipedia.org/wiki/Algebraic_data_type

## Highlights

 - **Type-safe.** Such things as improperly typed variants, non-exhaustive pattern matching, and invalid field access are caught at compile-time.

 - **Portable.** Everything you need is a standard-conforming C99 compiler; neither the standard library, nor compiler/platform-specific functionality or VLA are required.

 - **Predictable.** Datatype99 comes with formal [code generation semantics], meaning that the generated data layout is guaranteed to always be the same.

 - **Comprehensible errors.** Datatype99 is [resilient to bad code].

[resilient to bad code]: #q-what-about-compile-time-errors

## Installation

 1. Download Datatype99 and [Metalang99] (minimum supported version -- [1.9.0](https://github.com/Hirrolot/metalang99/releases/tag/v1.9.0)).
 2. Add `datatype99` and `metalang99/include` to your include paths.
 3. `#include <datatype99.h>` beforehand.

Some handy advices:

 - **PLEASE**, use Datatype99 only with [`-ftrack-macro-expansion=0`] (GCC), [`-fmacro-backtrace-limit=1`] (Clang), or something similar, otherwise it will throw your compiler to the moon.

 - [Precompile headers] that use Datatype99 so that they will not be compiled each time they are included. It is helpful to reduce compilation times, but they are not mandatory.

[Metalang99]: https://github.com/Hirrolot/metalang99
[Precompile headers]: https://en.wikipedia.org/wiki/Precompiled_header
[`-ftrack-macro-expansion=0`]: https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html
[`-fmacro-backtrace-limit=1`]: https://clang.llvm.org/docs/ClangCommandLineReference.html#cmdoption-clang-fmacro-backtrace-limit

## Usage

Put simply, Datatype99 is just a syntax sugar over [tagged unions]; the only difference is that it is more safe and concise. For example, to represent a binary tree, you would normally write something like this:

```c
typedef struct {
    struct BinaryTree *lhs;
    int x;
    struct BinaryTree *rhs;
} BinaryTreeNode;

typedef struct {
    enum { Leaf, Node } tag;
    union {
        int leaf;
        BinaryTreeNode node;
    } data;
} BinaryTree;
```

To avoid this boilerplate, you can use Datatype99:

```c
datatype(
    BinaryTree,
    (Leaf, int),
    (Node, BinaryTree *, int, BinaryTree *)
);
```

Say you want to sum all nodes and leafs in your binary tree. Then you may write something like this:

```c
int sum(const BinaryTree *tree) {
    switch (tree->tag) {
    case Leaf:
        return tree->data.leaf;
    case Node:
        return sum(tree->data.node.lhs) + tree->data.node.x + sum(tree->data.node.rhs);
    }
}
```

... but what if you accidentally access `tree->data.node` after `case Leaf:`? Your compiler would not warn you, thus resulting in a business logic bug.

With Datatype99, you can rewrite `sum` as follows, using a technique called _pattern matching_:

```c
int sum(const BinaryTree *tree) {
    match(*tree) {
        of(Leaf, x) return *x;
        of(Node, lhs, x, rhs) return sum(*lhs) + *x + sum(*rhs);
    }
}
```

`of` gives you variables called _bindings_: `x`, `lhs`, or `rhs`. This design has a few neat aspects:

 - **Compile-time safety.** The bindings of `Node` are invisible after `of(Leaf, x)` and vice versa, so compilation will fail to proceed if you access them inappropriately.
 - **Flexibility.** Bindings have pointer types so that you can mutate them, thereby mutating the whole `tree`; in order to obtain a value, you can dereference them, as shown in the example: `return *x;`.

The last thing unmentioned is how you construct variants. Internally, Datatype99 generates `inline static` functions called _value constructors_; you can use them as follows:

```c
BinaryTree leaf5 = Leaf(5);
BinaryTree leaf7 = Leaf(7);
BinaryTree node = Node(&leaf5, 123, &leaf7);
```

Finally, just a few brief notes about pattern matching:

 - To match the default case, write `otherwise { ... }` at the end of `match`.
 - To ignore a binding, write `_`: `of(Foo, a, b, _, d)`.
 - **PLEASE**, [do **not** use top-level `break`/`continue`](#top-level-breakcontinue) inside statements provided to `of` and `ifLet`; use `goto` labels instead.

Congratulations, this is all you need to know to write most of the stuff! If you feel fancy, you can also introspect your types at compile-time; see [`examples/derive/`](examples/derive/) for the examples.

Happy hacking!

[tagged unions]: https://en.wikipedia.org/wiki/Tagged_union

## Syntax and semantics

Having a well-defined semantics of the macros, you can write an FFI which is quite common in C.

### EBNF syntax

```ebnf
<datatype>      ::= "datatype(" [ <derive-clause> "," ] <datatype-name> { "," <variant> }+ ")" ;
<record>        ::= "record("   [ <derive-clause> "," ] <record-name>   { "," <field>   }* ")" ;
<datatype-name> ::= <ident> ;
<record-name>   ::= <ident> ;

<variant>       ::= "(" <variant-name> { "," <type> }* ")" ;
<field>         ::= "(" <type> "," <field-name> ")" ;
<variant-name>  ::= <ident> ;
<field-name>    ::= <ident> ;

<derive-clause> ::= "derive(" <deriver-name> { "," <deriver-name> }* ")" ;
<deriver-name>  ::= <ident> ;

<match>         ::= "match(" <lvalue> ") {" { <of> }* [ <otherwise> ] "}" ;
<matches>       ::= "MATCHES(" <expr> "," <ident> ")" ;
<if-let>        ::= "ifLet(" <lvalue> "," <variant-name> "," <ident> { "," <ident> }* ")" <stmt> ;
<of>            ::= "of(" <variant-name> { "," <ident> }* ")" <stmt> ;
<otherwise>     ::= "otherwise" <stmt> ;
```

<details>
  <summary>Note: shortened vs. postfixed versions</summary>

  Each listed identifier in the above grammar corresponds to a macro name defined by default -- these are called _shortened versions_. On the other hand, there are also _postfixed versions_ (`match99`, `of99`, `derive99`, etc.), which are defined unconditionally. If you want to avoid name clashes caused by shortened versions, define `DATATYPE99_NO_ALIASES` before including `datatype99.h`. Library headers are strongly advised to use the postfixed macros, but without resorting to `DATATYPE99_NO_ALIASES`.
</details>
  
### Semantics

(It might be helpful to look at the [generated data layout](https://godbolt.org/z/3TKn8T3Gj) of [`examples/binary_tree.c`](examples/binary_tree.c).)

#### `datatype`

 1. Before everything, the following type definition is generated:

```
typedef struct <datatype-name> <datatype-name>;
```

 2. For each non-empty variant, the following type definition is generated (the metavariable `<type>` ranges over a corresponding variant's types):

```
typedef struct <datatype-name><variant-name> {
    <type>0 _0;
    ...
    <type>N _N;
} <datatype-name><variant-name>;
```

 3. For each non-empty variant, the following type definitions to types of each field of `<datatype-name><variant-name>` are generated:

```
typedef <type>0 <variant-name>_0;
...
typedef <type>N <variant-name>_N;
```

 4. For each variant, the following type definition to a corresponding sum type is generated:

```
typedef struct <datatype-name> <variant-name>SumT;
```

 5. For each sum type, the following tagged union is generated (inside the union, only fields to structures of non-empty variants are generated):

```
typedef enum <datatype-name>Tag {
    <variant-name>0Tag, ..., <variant-name>NTag
} <datatype-name>Tag;

typedef union <datatype-name>Variants {
    char dummy;

    <datatype-name><variant-name>0 <variant-name>0;
    ...
    <datatype-name><variant-name>N <variant-name>N;
} <datatype-name>Variants;

struct <datatype-name> {
    <datatype-name>Tag tag;
    <datatype-name>Variants data;
};
```

<details>
  <summary>Note on char dummy;</summary>

`char dummy;` is needed to make the union contain at least one item, according to the standard, even if all variants are empty. Such a `datatype` would enforce strict type checking unlike plain C `enum`s.
</details>

 6. For each variant, the following function called a _value constructor_ is generated:

```
inline static <datatype-name> <variant-name>(/* ... */) { /* ... */ }
```

 7. Now, when a sum type is fully generated, the derivation process takes place. Each deriver taken from `derive(...)` is invoked sequentially, from left to right, as

```
ML99_call(DATATYPE99_DERIVE_##<deriver-name>I, v(<datatype-name>), variants...)
```

where
 - `<deriver-name>I` corresponds to a [Metalang99-compliant] macro of the form `#define DATATYPE99_DERIVE_##<deriver-name>I_IMPL(name, variants) /* ... */`.
 - `variants...` is a [list] of variants represented as two-place [tuples]: `(<variant-name>, types...)`, where
   - `types...` is a [list] of types of the corresponding variant.

Put simply, a deriver is meant to generate something global for a sum type, like interface implementations or almost any other stuff. In terms of Rust, you can think of it as of the [`derive` attribute].

[list]: https://metalang99.readthedocs.io/en/latest/list.html
[tuples]: https://metalang99.readthedocs.io/en/latest/tuple.html
[`derive` attribute]: https://doc.rust-lang.org/reference/attributes/derive.html

#### `record`

`record` represents a _record type_: it is simply a `struct` for which the derivation process is defined.

 1. The following structure is generated:

```
typedef struct <record-name> {
    // Only if <record-name> has no fields:
    char dummy;

    <type>0 <field-name>0;
    ...
    <type>N <field-name>N;
} <record-name>;
```

<details>
  <summary>Note on char dummy;</summary>

`char dummy;` is needed to make the structure contain at least one item, according to the standard. Such `record(Foo)` can be used to implement interfaces for it (see [Interface99]).
</details>

[Interface99]: https://github.com/Hirrolot/interface99

 2. Each deriver taken from `derive(...)` is invoked sequentially, from left to right, as

```
ML99_call(DATATYPE99_RECORD_DERIVE_##<deriver-name>I, v(<record-name>), fields...)
```

where
 - `<deriver-name>I` corresponds to a [Metalang99-compliant] macro of the form `#define DATATYPE99_RECORD_DERIVE_##<deriver-name>I_IMPL(name, fields) /* ... */`.
 - `fields...` is a [list] of fields represented as two-place [tuples]: `(<type>, <field-name>)`. If a record contains no fields, the list would consist only of `(char, dummy)`.

#### `match`

`match` has the expected semantics: it sequentially tries to match the given instance of a sum type against the given variants, and, if a match has succeeded, it executes the corresponding statement and moves down to the next instruction (`match(val) { ... } next-instruction;`). If all the matches have failed, it executes the statement after `otherwise` and moves down to the next instruction.

A complete `match` construct results in a single C statement.

#### `of`

`of` accepts a matched variant name as a first argument and the rest of arguments comprise a comma-separated list of bindings.

 - A binding equal to `_` is ignored.
 - A binding **not** equal to `_` stands for a pointer to a corresponding data of the variant (e.g., let there be `(Foo, T1, T2)` and `of(Foo, x, y)`, then `x` has the type `T1 *` and `y` is `T2 *`).

There can be more than one `_` binding, however, non-`_` bindings must be distinct.

To match an empty variant, write `of(Bar)`.

#### `MATCHES`

`MATCHES` just tests an instance of a sum type for a given variant. If the given instance corresponds to the given variant, it expands to truthfulness, otherwise it expands to falsehood.

#### `matches`

**DEPRECATED**: use [`MATCHES`](#MATCHES) instead.

#### `ifLet`

`ifLet` tries to match the given instance of a sum type against the given variant, and, if a match has succeeded, it executes the corresponding statement.

Think of `ifLet(<expr>, <variant-name>, vars...) { /* ... */ }` as of an abbreviation of

```
match(<expr>) {
    of(<variant-name>, vars...) { /* ... */ }
    otherwise {}
}
```

A complete `ifLet` construct results in a single C statement.

## Unit type

The unit type `UnitT99` represents the type of a single value, `unit_v99` (it should not be assigned to anything else). These are defined as follows:

```c
typedef char UnitT99;
static const UnitT99 unit_v99 = '\0';
```

If `DATATYPE99_NO_ALIASES` remains undefined prior to `#include <datatype99.h>`, `UnitT99` and `unit_v99` are also accessible through object-like macros `UnitT` & `unit_v`.

## Derive helper attributes

You can pass named arguments to a deriver; these are called _derive helper attributes_. They must be specified as object-like macros of the form:

```
#define <variant-name>_<namespace>_<attribute-name> attr(/* attribute value */)
```

where `<namespace>` is either `<datatype-name>`/`<record-name>` or `<variant-name>`/`<field-name>` for `datatype`/`record`-specific and variant/field-specific attributes, respectively.

To manipulate derive helper attributes, there are a few predefined macros:

 - `DATATYPE99_attrIsPresent`/`DATATYPE99_ATTR_IS_PRESENT`

    Accepts an attribute name and checks if it is present or not. It can be used to check the presence of an optional attribute.

 - `DATATYPE99_attrValue`/`DATATYPE99_ATTR_VALUE`

    Accepts an attribute name extracts its value. A provided attribute **must** be present.

 - `DATATYPE99_assertAttrIsPresent`

    Accepts an attribute name and emits a fatal error if the attribute is not present, otherwise results in emptiness. It can be used for mandatory attributes.

(The naming convention here is the same [as of Metalang99](https://metalang99.readthedocs.io/en/latest/#naming-conventions).)

## Miscellaneous

 - The macros `DATATYPE99_MAJOR`, `DATATYPE99_MINOR`, `DATATYPE99_PATCH`, `DATATYPE99_VERSION_COMPATIBLE(x, y, z)`, and `DATATYPE99_VERSION_EQ(x, y, z)` have the [same semantics as of Metalang99](https://metalang99.readthedocs.io/en/latest/#version-manipulation-macros).

 - For each macro using `ML99_EVAL`, Datatype99 provides its [Metalang99-compliant] counterpart which can be used inside derivers and other Metalang99-compliant macros:

| Macro | Metalang99-compliant counterpart |
|----------|----------|
| `datatype` | `DATATYPE99_datatype` |
| `record` | `DATATYPE99_record` |
| `of` | `DATATYPE99_of` |
| `ifLet` | `DATATYPE99_ifLet` |

(An [arity specifier] and [desugaring macro] are provided for each of the above macros.)

 - There is a built-in deriver `dummy` which generates nothing. It is defined both for record and sum types.

[SemVer]: https://semver.org
[Metalang99-compliant]: https://metalang99.readthedocs.io/en/latest/#definitions
[arity specifier]: https://hirrolot.gitbook.io/metalang99/partial-application
[desugaring macro]: https://metalang99.readthedocs.io/en/latest/#definitions

## Guidelines

### Clang-Format issues

If you use [Clang-Format], cancel formatting for a `datatype` definition using `// clang-format off` & `// clang-format on` to make it look prettier, as in the examples.

### `#undef` derive helper attributes

Always `#undef` derive helper attributes after a corresponding `datatype` definition not to pollute your namespace.

### Descriptive names

If the meaning of variant parameters is not clear from the context, give them descriptive names. This can be achieved in several ways:

```c
// 1. Define type aliases to variant parameters.
typedef double XCoordinate;
typedef double YCoordinate;

typedef double Width;
typedef double Height;

datatype(
    Shape,
    (Point, XCoordinate, YCoordinate),
    (Rectangle, Width, Height)
);

// 2. Define separate structures.
typedef struct {
    double x, y;
} Point;

typedef struct {
    double width, height;
} Rectangle;

datatype(
    Shape,
    (MkPoint, Point),
    (MkRectangle, Rectangle)
);
```

Comparison:

 - The former option has more concise syntax: `MkPoint(x, y)` instead of `MkPoint((Point){x, y})`.
 - The latter option is more appropriate when the structures are to be used separately from the containing sum type.
 - The latter option allows for more graduate control over the data layout: you can accompain the structures with compiler-specific attributes, alignment properties like `__attribute__ ((__packed__))`, etc.

[Clang-Format]: https://clang.llvm.org/docs/ClangFormatStyleOptions.html

## Pitfalls

### Top-level `break`/`continue`

Do **not** use `break`/`continue` inside a statement provided to `of`/`ifLet` but outside of any `for`/`while` loops in that statement. For example, this code is fine:

```c
match(x) {
    of(Foo, a, b, c) {
        for (int i = 0; i < 10; i++) {
            continue;
        }
    }
}
```

But this code is **not** fine:

```c
for (int i = 0; i < 10; i++) {
    match(x) {
        of(Foo, a, b, c) {
            if (a == 7) { break; }
            continue;
        }
    }
}
```

To make it valid, you can rewrite it as follows:

```c
for (int i = 0; i < 10; i++) {
    match(x) {
        of(Foo, a, b, c) {
            if (a == 7) { goto my_break; }
            goto my_continue;
        }
    }

    // Datatype99 prohibits top-level `break`/`continue`.
    my_continue:;
}
my_break:;
```

### Array as a variant parameter

To specify an array as a variant parameter, you must put it into a separate `struct`; see [`examples/array_in_variant.c`](examples/array_in_variant.c).

### Mutable bindings

Bindings introduced by `of` are **always** mutable, so make sure you do **not** mutate them if the value passed to `match` is qualified as `const`.

## Credits

Thanks to Rust and ML for their implementations of sum types.

## Publications

 - [_What’s the Point of the C Preprocessor, Actually?_](https://hirrolot.github.io/posts/whats-the-point-of-the-c-preprocessor-actually.html) by Hirrolot.
 - [_Macros on Steroids, Or: How Can Pure C Benefit From Metaprogramming_](https://hirrolot.github.io/posts/macros-on-steroids-or-how-can-pure-c-benefit-from-metaprogramming.html) by Hirrolot.
 - [_Extend Your Language, Don’t Alter It_](https://hirrolot.github.io/posts/extend-your-language-dont-alter-it.html) by Hirrolot.
 - [_Compiling Algebraic Data Types in Pure C99_] by Hirrolot.
 - [_Comparing Rust and Datatype99_](https://www.reddit.com/r/ProgrammingLanguages/comments/nc1o18/comparing_algebraic_data_types_rust_and_datatype99/) by Hirrolot.
 - [_Compile-Time Introspection of Sum Types in Pure C99_](https://hirrolot.github.io/posts/compile-time-introspection-of-sum-types-in-pure-c99.html) by Hirrolot.
 - [_Unleashing Sum Types in Pure C99_](https://hirrolot.github.io/posts/unleashing-sum-types-in-pure-c99.html) by Hirrolot.

[_Compiling Algebraic Data Types in Pure C99_]: https://hirrolot.github.io/posts/compiling-algebraic-data-types-in-pure-c99.html

## Release procedure

 1. Update `DATATYPE99_MAJOR`, `DATATYPE99_MINOR`, and `DATATYPE99_PATCH` in `datatype99.h`.
 2. Update `CHANGELOG.md`.
 3. Release the project in [GitHub Releases].

[GitHub Releases]: https://github.com/Hirrolot/datatype99/releases

## FAQ

### Q: Why use C instead of Rust/Zig/whatever else?

A: There is a lot of software written in plain C that can benefit from Datatype99; C is #1 programming language as of 2020, [according to TIOBE](https://jaxenter.com/c-programming-may-2020-171598.html). People use C due to technical and social reasons:

 - Datatype99 can be seamlessly integrated into existing codebases written in pure C -- just `#include <datatype99.h>` and you are ready to go. On the other hand, other languages force you to separate native C files from their sources, which is clearly less convenient.

 - In some environments, developers strick to pure C for historical reasons (e.g., embedded devices, Linux and other operating systems).

 - C has a stable ABI which is vital for some projects (e.g., plugin systems such as [MetaCall]).

 - C is a mature language with a complete specification and a plenitude of libraries. Rust has no complete specification, and [Zig] is not yet production-ready. I know a few stories when these two languages were rejected for new projects, and I can understand this decision.

 - Historically, C has been targeting nearly all platforms. This is not the case with Rust, which depends on LLVM as for now.

 - Your company obligates you to use C.

 - Etc.

[MetaCall]: https://github.com/metacall/core
[Zig]: https://ziglang.org/

See also:
 - [_Rust is not a good C replacement_](https://drewdevault.com/2019/03/25/Rust-is-not-a-good-C-replacement.html) by Drew DeVault.

Overall, if you can afford a more modern/high-level language, I encourage you to do so instead of using old C. However, many people do not have this possibility (or it would be too costly).

### Q: Why not third-party code generators?

A: See [Metalang99's README >>](https://github.com/Hirrolot/metalang99#q-why-not-third-party-code-generators).

### Q: How does it work?

A: In short, `datatype` expands to a tagged union with value constructors; `match` expands to a switch statement. To generate all this stuff, [Metalang99] is used, a preprocessor metaprogramming library.

More on it in [_Compiling Algebraic Data Types in Pure C99_].

### Q: Does it work on C++?

A: Yes, C++11 and onwards is supported.

### Q: What is the difference between Datatype99 and Metalang99?

A: [Metalang99] is a functional language for metaprogramming, whereas Datatype99 is an implementation of algebraic data types written in this language.

### Q: What about compile-time errors?

A: Some kinds of syntactic errors are detected by the library itself:

#### Error: `Bar(int)` instead of `(Bar, int)`

[`playground.c`]
```c
datatype(A, (Foo, int), Bar(int));
```

[`/bin/sh`]
```
$ gcc playground.c -Imetalang99/include -Idatatype99 -ftrack-macro-expansion=0
playground.c:3:1: error: static assertion failed: "ML99_assertIsTuple: Bar(int) must be (x1, ..., xN)"
    3 | datatype(A, (Foo, int), Bar(int));
      | ^~~~~~~~
```

----------

#### Error: Missing comma

[`playground.c`]
```c
datatype(A, (Foo, int) (Bar, int));
```

[`/bin/sh`]
```
$ gcc playground.c -Imetalang99/include -Idatatype99 -ftrack-macro-expansion=0
playground.c:3:1: error: static assertion failed: "ML99_assertIsTuple: (Foo, int) (Bar, int) must be (x1, ..., xN), did you miss a comma?"
    3 | datatype(A, (Foo, int) (Bar, int));
      | ^~~~~~~~
```

----------

#### Error: Trailing comma is prohibited

[`playground.c`]
```c
datatype(A, (Foo, int), (Bar, int), /* trailing comma is prohibited */);
```

[`/bin/sh`]
```
$ gcc playground.c -Imetalang99/include -Idatatype99 -ftrack-macro-expansion=0
playground.c:3:1: error: static assertion failed: "ML99_assertIsTuple: must be (x1, ..., xN)"
    3 | datatype(A, (Foo, int), (Bar, int), /* trailing comma is prohibited */);
      | ^~~~~~~~
```

(For better diagnostics, use the latest Metalang99.)

The others are understandable as well:

#### Error: unknown type name specified in `datatype`

[`playground.c`]
```c
datatype(Foo, (FooA, NonExistingType));
```

[`/bin/sh`]
```
playground.c:3:1: error: unknown type name ‘NonExistingType’
    3 | datatype(
      | ^~~~~~~~
playground.c:3:1: error: unknown type name ‘NonExistingType’
playground.c:3:1: error: unknown type name ‘NonExistingType’
```

----------

#### Error: non-exhaustive `match`

[`playground.c`]
```c
match(*tree) {
    of(Leaf, x) return *x;
    // of(Node, lhs, x, rhs) return sum(*lhs) + *x + sum(*rhs);
}
```

[`/bin/sh`]
```
playground.c: In function ‘sum’:
playground.c:6:5: warning: enumeration value ‘NodeTag’ not handled in switch [-Wswitch]
    6 |     match(*tree) {
      |     ^~~~~
```

----------

#### Error: excess binders in `of`

[`playground.c`]
```c
match(*tree) {
    of(Leaf, x, excess) return *x;
    of(Node, lhs, x, rhs) return sum(*lhs) + *x + sum(*rhs);
}
```

[`/bin/sh`]
```
playground.c: In function ‘sum’:
playground.c:15:9: error: unknown type name ‘Leaf_1’; did you mean ‘Leaf_0’?
   15 |         of(Leaf, x, excess) return *x;
      |         ^~
      |         Leaf_0
playground.c:15:9: error: ‘BinaryTreeLeaf’ has no member named ‘_1’; did you mean ‘_0’?
   15 |         of(Leaf, x, excess) return *x;
      |         ^~
      |         _0
```

----------

#### Error: improperly typed variant arguments

[`playground.c`]
```c
BinaryTree tree = Leaf("hello world");
```

[`/bin/sh`]
```
playground.c: In function ‘main’:
playground.c:18:28: warning: passing argument 1 of ‘Leaf’ makes integer from pointer without a cast [-Wint-conversion]
   18 |     BinaryTree tree = Leaf("hello world");
      |                            ^~~~~~~~~~~~~
      |                            |
      |                            char *
playground.c:6:1: note: expected ‘int’ but argument is of type ‘char *’
    6 | datatype(
      | ^~~~~~~~
```

----------

#### Error: an undereferenced binder

[`playground.c`]
```c
int sum(const BinaryTree *tree) {
    match(*tree) {
        of(Leaf, x) return x; // x is int *
        of(Node, lhs, x, rhs) return sum(*lhs) + *x + sum(*rhs);
    }
}
```

[`/bin/sh`]
```
playground.c: In function ‘sum’:
playground.c:17:28: warning: returning ‘Leaf_0 *’ {aka ‘int *’} from a function with return type ‘int’ makes integer from pointer without a cast [-Wint-conversion]
   17 |         of(Leaf, x) return x; // x is int *
      |                            ^
```

----------

From my experience, nearly 95% of errors make sense.

If an error is not comprehensible at all, try to look at generated code (`-E`). Hopefully, the [code generation semantics] is formally defined so normally you will not see something unexpected.

[code generation semantics]: #semantics

### Q: What about IDE support?

![Suggestion](images/suggestion.png)

A: VS Code automatically enables suggestions of generated types but, of course, it does not support macro syntax highlightment.

### Q: Which compilers are tested?

A: Datatype99 is known to work on these compilers:

 - GCC
 - Clang
 - MSVC
 - TCC

## Troubleshooting

### `warning: control reaches end of non-void function [-Wreturn-type]`

This is a known false positive occurring when `match` is used to return control back to a caller. Unfortunately, we cannot fix it in the library itself, but there are two workarounds:

 1. Disable this warning explicitly. With [GCC diagnostic pragmas] (or the [Clang's counterpart](https://clang.llvm.org/docs/UsersManual.html#controlling-diagnostics-via-pragmas)):

```c
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int foo(void) {
    match(x) {
        of(Foo, foo) return X;
        of(Bar, bar) return Y;
    }
}
#pragma GCC diagnostic pop
```

[GCC diagnostic pragmas]: https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html

 2. Assign a result variable inside branches and return it after `match`:

```c
int foo(void) {
    int result = 0;

    match(x) {
        of(Foo, foo) result = X;
        of(Bar, bar) result = Y;
    }

    return result;
}
```

See [issue 9](https://github.com/Hirrolot/datatype99/issues/9).
