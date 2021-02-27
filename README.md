<div align="center">
  <img src="preview.png" width="650"/>
  <h1>Datatype99</h1>
  <a href="https://github.com/Hirrolot/datatype99/actions">
    <img src="https://github.com/Hirrolot/datatype99/workflows/C/C++%20CI/badge.svg">
  </a>

  A header-only library featuring safe, intuitive [sum types] with exhaustive pattern matching.
</div>

[sum types]: https://en.wikipedia.org/wiki/Tagged_union

## Table of contents

 - [Highlights](#highlights)
 - [Installation](#installation)
 - [Usage](#usage)
 - [Syntax and semantics](#syntax-and-semantics)
   - [EBNF syntax](#ebnf-syntax)
   - [Semantics](#semantics)
     - [`datatype99`](#datatype99-1)
     - [`match99`](#match99)
     - [`of99`](#of99)
     - [`matches99`](#matches99)
     - [Unit type](#unit-type)
 - [Credits](#credits)
 - [Learning resources](#learning-resources)
 - [FAQ](#faq)
 - [Troubleshooting](#troubleshooting)

## Highlights

 - **Type-safe.** Unlike manually written tagged unions, Datatype99 is type-safe: normally you cannot access invalid data or construct an invalid variant. Pattern matching is exhaustive too.

 - **Pure C99.** No external tools are required -- Datatype99 is implemented using only preprocessor macros.

 - **Can be used everywhere.** Literally everywhere provided that you have a standard-confirming C99 preprocessor. Even on freestanding environments.

 - **Transparent.** Datatype99 comes with formal [code generation semantics], meaning if you try to look at `datatype`'s output, normally you will not see something unexpected.

 - **FFI-tolerant.** Because of transparency, writing an FFI is not a challenge.

## Installation

 1. Download Datatype99 and [Metalang99] (minimum supported version -- [0.4.0](https://github.com/Hirrolot/metalang99/releases/tag/v0.4.0)).
 2. Add `datatype99` and `metalang99/include` to your include paths.
 3. `#include <datatype99.h>` beforehand.

**PLEASE**, use Datatype99 only with [`-ftrack-macro-expansion=0`] (GCC) or something similar, otherwise it will throw your compiler to the moon. [Precompiled headers] are also very helpful.

If you do **not** want the shortened versions to appear (e.g., `datatype` and `match` instead of `datatype99` and `match99`), define `DATATYPE99_NO_ALIASES` before `#include <datatype99.h>`.

[Metalang99]: https://github.com/Hirrolot/metalang99
[precompiled headers]: https://en.wikipedia.org/wiki/Precompiled_header
[`-ftrack-macro-expansion=0`]: https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html

## Usage

(The full example: [`examples/binary_tree.c`](examples/binary_tree.c).)

A sum type is created using the `datatype` macro. I guess you have already caught the syntax but actually there exist one more kind of a variant: an empty variant which is expressed simply as `(Foo)`. It holds no data.

Pattern matching is likewise intuitive. Just three brief notes:

 - To match an empty variant, write `of(Foo) { ... }`.
 - To match the default case, i.e. when all other cases failed, write `otherwise { ... }`.
 - To ignore one or more variables inside `of`, write `of(Foo, a, b, _, d)`.

Happy hacking!

## Syntax and semantics

Having a well-defined semantics of the macros, you can write an FFI which is quite common in C.

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

(It might be helpful to look at the [generated code](https://godbolt.org/z/zhj69Y) of [`examples/binary_tree.c`](examples/binary_tree.c)'s `BinaryTree`.)

#### `datatype99`

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
struct <datatype-name> {
    enum {
        <variant-name>0Tag, ..., <variant-name>NTag
    } tag;

    union {
        char dummy[1];
        <datatype-name><variant-name>0 <variant-name>0;
        ...
        <datatype-name><variant-name>N <variant-name>N;
    } data;
};
```

 6. For each variant, the following function called a _value constructor_ is generated:

```
inline static <datatype99-name> <variant-name>(...) { /* ... */ }
```

### `match99`

`match99` has the expected semantics: it sequentially tries to match the given instance of a sum type against the given variants, and, if a match has succeeded, it executes the corresponding statement and moves down to the next instruction (`match(val) { ... } next-instruction;`). If all the matches have failed, it executes the statement after `otherwise99` and moves down to the next instruction.

### `of99`

`of99` accepts a matched variant name as a first argument and the rest of arguments comprise a comma-separated list of bindings.

 - A binding equal to `_` is ignored.
 - A binding **not** equal to `_` stands for a pointer to a corresponding data of the variant (e.g., let there be `(Foo, T1, T2)` and `of99(Foo, x, y)`, then `x` has the type `T1 *` and `y` is `T2 *`).

There can be more than one `_` binding, however, non-`_` bindings must be distinct.

To match an empty variant, write `of99(Bar)`.

### `matches99`

`matches99` just tests an instance of a sum type for a given variant. If the given instance corresponds to the given variant, it expands to truthfulness, otherwise it expands to falsehood.

### Unit type

The unit type `Unit99` represents a type of a single value, `unit99` (it should not be assigned to anything else). `Unit99` and `unit99` are defined as follows:

```c
typedef char Unit99;
static const Unit99 unit99 = '\0';
```

## Credits

Thanks to Rust and ML for their implementations of sum types.

## Learning resources

 - [_Unleashing Sum Types in Pure C99_](https://medium.com/@hirrolot/unleashing-sum-types-in-pure-c99-31544302d2ba) by Hirrolot

## FAQ

### Q: How does it work?

A: The `datatype99` macro generates a tagged union accompanied with type hints and value constructors. Pattern matching desugars merely to a switch statement. To generate all this stuff, [Metalang99] is used, which is a preprocessor metaprogramming library.

### Q: What about compile-time errors?

A: With `-ftrack-macro-expansion=0` (GCC), there are no chances that compile-time errors will be longer than usual. Some kinds of syntactic errors are detected by the library itself, for example (`-E` flag):

```c
// !"Metalang99 error" (datatype99): "Bar(int) is unparenthesised"
datatype(A, (Foo, int), Bar(int));
```

The others are understandable as well:

```c
datatype(Foo, (FooA, NonExistingType));
```

```
playground.c:3:1: error: unknown type name ‘NonExistingType’
    3 | datatype(
      | ^~~~~~~~
playground.c:3:1: error: unknown type name ‘NonExistingType’
playground.c:3:1: error: unknown type name ‘NonExistingType’
```

If an error is not comprehensible at all, try to look at generated code (`-E`). Hopefully, the [code generation semantics] is formally defined so normally you will not see something unexpected.

[code generation semantics]: #semantics

### Q: How to use an array inside a variant?

A: See [`examples/array_in_variant.c`](examples/array_in_variant.c).

### Q: Why not just use third-party code generators?

A: See [Metalang99's README](https://github.com/Hirrolot/metalang99#q-why-not-just-use-third-party-code-generators).

## Troubleshooting

### Problem: I can't make it work on MSVC

Solution: See [Metalang99's README](https://github.com/Hirrolot/metalang99#problem-i-cant-make-it-work-on-msvc).
