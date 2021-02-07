<div align="center">
  <img src="preview.png" width="550"/>
  <h1>datatype99</h1>
  <a href="https://github.com/Hirrolot/datatype99/actions">
    <img src="https://github.com/Hirrolot/datatype99/workflows/C/C++%20CI/badge.svg">
  </a>
  <a href="https://patreon.com/hirrolot">
    <img src="https://img.shields.io/endpoint.svg?url=https%3A%2F%2Fshieldsio-patreon.vercel.app%2Fapi%3Fusername%3Dhirrolot%26type%3Dpatrons&style=flat">
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
 - [Credits](#credits)
 - [Learning resources](#learning-resources)
 - [FAQ](#faq)

## Highlights

 - **Type-safe.** Unlike manually written tagged unions, datatype99 is type-safe: normally you cannot access invalid data or construct an invalid variant. Pattern matching is exhaustive too.

 - **Pure C99.** No external tools are required; datatype99 is implemented using only preprocessor macros.

 - **Zero boilerplate.** datatype99 deals with all the dirty stuff.

 - **Can be used everywhere.** Literally everywhere provided that you have a standard-confirming C99 preprocessor. Even on freestanding environments.

 - **Intuitive.** You already know how to use it!

## Installation

 1. Download datatype99 and [metalang99].
 2. Add `datatype99` and `metalang99/include` to your include paths.

To use datatype99, just `#include <datatype99.h>` beforehand. No additional setup is needed: datatype99 is implemented using only preprocessor macros.

To speed up compilation, consider using [precompiled headers] and `-ftrack-macro-expansion=0` (GCC-only). The last option is especially useful because it tells GCC not to print big bullshit macro errors.

If you do **not** want the shortened versions to appear (e.g., `datatype` and `match` instead of `datatype99` and `match99`), define `DATATYPE99_NO_ALIASES` before `#include <datatype99.h>`.

[metalang99]: https://github.com/Hirrolot/metalang99
[precompiled headers]: https://en.wikipedia.org/wiki/Precompiled_header

## Usage

(The full example: [`examples/binary_tree.c`](examples/binary_tree.c).)

A sum type is created using the `datatype` macro. I guess you have already caught the syntax but actually there exist one more kind of a variant: an empty variant which is expressed simply as `(Foo)`. It holds no data.

Pattern matching is likewise intuitive. Just two brief notes:

 - To match an empty variant, write `of(Foo) { ... }`.
 - To match the default case, i.e. when all other cases failed, write `otherwise { ... }`.

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

#### `datatype99`

This macro accepts a sum type name as a first argument and the rest of arguments shall be comma-separated variants.

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
        char dummy;
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

This macro implements [pattern matching] for an instance of a sum type. It accepts an expression of a sum type as a single argument. Afterwards, a chain of arms shall follow.

`match99` has the expected semantics: it tries to match the given instance of a sum type with the given variants, and, if a match has succeeded, it executes the corresponding statement and moves down to the next instruction. If all matches have failed, it executes the statement after `otherwise99` and moves down to the next instruction.

### `of99`

This macro accepts a variant name as a first argument and the rest of arguments comprise a list of variable names. Each variable name is a pointer to a corresponding data of the variant (e.g., let there be `(Foo, T1, T2)` and `of99(Foo, x, y)`, then `x` has the type `T1 *` and `y` is `T2 *`). To match an empty variant, write `of99(Bar)`.

### `matches99`

This macro just tests an instance of a sum type for a given variant. If the given instance corresponds to the given variant, it expands to truthfulness, otherwise, it expands to falsehood.

[pattern matching]: https://en.wikipedia.org/wiki/Pattern_matching

## Credits

Thanks to Rust and ML for their implementations of sum types.

## Learning resources

 - [_Unleashing Sum Types in Pure C99_](https://medium.com/@hirrolot/unleashing-sum-types-in-pure-c99-31544302d2ba) by Hirrolot

## FAQ

### Q: What about compile-time errors?

A: With `-ftrack-macro-expansion=0` (GCC), there are no chances that compile-time errors will be longer than usual (remember templates!). However, they can be still quite obscured -- in this case, try to look at generated code (`-E` flag). Hopefully, the [code generation semantics] is formally defined.

[code generation semantics]: #semantics

### Q: How to use an array inside a variant?

A: See [`examples/array_in_variant.c`](examples/array_in_variant.c).
