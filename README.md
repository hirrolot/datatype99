<div align="center">
  <img src="preview.png" width="600"/>
  <h1>Datatype99</h1>
  <a href="https://github.com/Hirrolot/datatype99/actions">
    <img src="https://github.com/Hirrolot/datatype99/workflows/C/C++%20CI/badge.svg">
  </a>

  Safe, intuitive [sum types] with exhaustive pattern matching & compile-time introspection facilities.
</div>

[sum types]: https://en.wikipedia.org/wiki/Tagged_union

## Highlights

 - **Type-safe.** Unlike manually written tagged unions, Datatype99 is type-safe: normally you cannot access invalid data or construct an invalid variant. Pattern matching is exhaustive too.

 - **Pure C99/C++11.** No external tools are required -- Datatype99 is implemented using only preprocessor macros.

 - **Can be used everywhere.** Literally everywhere provided that you have a standard-confirming C99/C++11 preprocessor. Even on freestanding environments.

 - **Transparent.** Datatype99 comes with formal [code generation semantics], meaning if you try to look at `datatype`'s output, normally you will not see something unexpected.

 - **FFI-tolerant.** Because of transparency, writing an FFI is not a challenge.

## Installation

 1. Download Datatype99 and [Metalang99] (minimum supported version -- [1.0.0](https://github.com/Hirrolot/metalang99/releases/tag/v1.0.0)).
 2. Add `datatype99` and `metalang99/include` to your include paths.
 3. `#include <datatype99.h>` beforehand.

**PLEASE**, use Datatype99 only with [`-ftrack-macro-expansion=0`] (GCC) or something similar, otherwise it will throw your compiler to the moon. [Precompiled headers] are also very helpful.

[Metalang99]: https://github.com/Hirrolot/metalang99
[precompiled headers]: https://en.wikipedia.org/wiki/Precompiled_header
[`-ftrack-macro-expansion=0`]: https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html

## Usage

(The full example: [`examples/binary_tree.c`](examples/binary_tree.c).)

A sum type is created using the `datatype` macro. I guess you have already caught the syntax but actually there exist one more kind of a variant: an empty variant which is expressed simply as `(Foo)`. It holds no data.

Pattern matching is likewise intuitive. Just a few brief notes:

 - To match an empty variant, write `of(Foo) { ... }`.
 - To match the default case, i.e. when all other cases failed, write `otherwise { ... }`.
 - To ignore one or more variables inside `of`, write `of(Foo, a, b, _, d)`.

Happy hacking!

## Syntax and semantics

Having a well-defined semantics of the macros, you can write an FFI which is quite common in C.

### EBNF syntax

```ebnf
<datatype>      ::= "datatype(" [ <derive-clause> "," ] <datatype-name> { "," <variant> }+ ")" ;

<variant>       ::= "(" <variant-name> { "," <type> }* ")" ;
<datatype-name> ::= <ident> ;
<variant-name>  ::= <ident> ;

<derive-clause> ::= "derive(" <deriver> { "," <deriver> }* ")" ;
<deriver>       ::= <deriver-name> | "(" <deriver-name> "," <deriver-args> ")" ;
<deriver-name>  ::= <ident> ;
<deriver-args>  ::= <pp-token-list> ;

<match>         ::= "match(" <lvalue> ")" { <arm> }+ ;
<matches>       ::= "matches(" <expr> "," <ident> ")" ;
<if-let>        ::= "ifLet(" <lvalue> "," <variant-name> "," <ident> { "," <ident> }* ")" <stmt> ;
<of>            ::= "of(" <variant-name> { "," <ident> }* ")" <stmt> ;
<otherwise>     ::= "otherwise" <stmt> ;
```

### Semantics

(It might be helpful to look at the [generated data layout](https://godbolt.org/z/vKaKqh46d) of [`examples/binary_tree.c`](examples/binary_tree.c).)

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

  (`char dummy;` is needed to make the union contain at least one item, according to the standard, even if all variants are empty. Such a `datatype` would enforce strict type checking unlike plain C `enum`s.)
</details>

 6. For each variant, the following function called a _value constructor_ is generated:

```
inline static <datatype-name> <variant-name>(...) { /* ... */ }
```

 7. Now, when a sum type is generated, the derivation process takes place. Each deriver is invoked sequentially, from left to right, either with extra arguments or without them, i.e.
    - `ML99_call(DATATYPE99_DERIVE_##<deriver-name>, v(<datatype-name>), variants...)` or
    - `ML99_call(DATATYPE99_DERIVE_##<deriver-name>, v(<datatype-name>), variants..., args...)`, where
       - `variants...` is a [list] of variants represented as two-place [tuples]: `(<variant-name>, types...)`, where
          - `types...` is a [list] of types of the corresponding variant.
       - `args...` are extra deriver arguments supplied in `(<deriver-name>, args...)`.

[list]: https://metalang99.readthedocs.io/en/latest/list.html
[tuples]: https://metalang99.readthedocs.io/en/latest/tuple.html

To specify attributes for a particular variant, follow this pattern:

```
#define <variant-name>_<deriver-name>_<attribute-name> attr(/* attribute value */)
```

<details>
  <summary>Note on this design decision</summary>

  (It is theoretically possible to specify an attribute right before the corresponding variant (as in Rust), but this would penetrate the performance and simplicity of the library.)
</details>

There are a few helping macros:

 - `DATATYPE99_ATTR_IS_PRESENT` accepts an attribute name and checks if it is present or not.
 - `DATATYPE99_ATTR_VALUE` accepts an attribute name and expands to its value. A provided attribute **must** be present.

Also, there is a built-in deriver called `dummy`, which can be specified either as `dummy` or `(dummy, (...))`; it generates nothing.

See [`examples/derive/`](examples/derive/) for examples of writing and using derivers.

#### `match`

`match` has the expected semantics: it sequentially tries to match the given instance of a sum type against the given variants, and, if a match has succeeded, it executes the corresponding statement and moves down to the next instruction (`match(val) { ... } next-instruction;`). If all the matches have failed, it executes the statement after `otherwise` and moves down to the next instruction.

A complete `match` construct results in a single C statement.

#### `of`

`of` accepts a matched variant name as a first argument and the rest of arguments comprise a comma-separated list of bindings.

 - A binding equal to `_` is ignored.
 - A binding **not** equal to `_` stands for a pointer to a corresponding data of the variant (e.g., let there be `(Foo, T1, T2)` and `of(Foo, x, y)`, then `x` has the type `T1 *` and `y` is `T2 *`).

There can be more than one `_` binding, however, non-`_` bindings must be distinct.

To match an empty variant, write `of(Bar)`.

#### `matches`

`matches` just tests an instance of a sum type for a given variant. If the given instance corresponds to the given variant, it expands to truthfulness, otherwise it expands to falsehood.

#### `ifLet`

`ifLet` tests for only one variant. It works the same as

```
match(<expr>) {
    of(<variant-name>, vars...) { /* ... */ }
    otherwise {}
}
```

, but has a shorter syntax:

```
ifLet(<expr>, <variant-name>, vars...) { /* ... */ }
```

A complete `ifLet` construct results in a single C statement, just as `match`.

### Unit type

The unit type `UnitT` represents a type of a single value, `unit_v` (it should not be assigned to anything else). `UnitT` and `unit_v` are defined as follows:

```c
typedef char UnitT;
static const UnitT unit_v = '\0';
```

### Miscellaneous

 - The macros `DATATYPE99_MAJOR`, `DATATYPE99_MINOR`, and `DATATYPE99_PATCH` stand for the corresponding components of a version of Datatype99.

 - If you do **not** want the shortened versions to appear (e.g. `match` without the prefix `99`), define `DATATYPE99_NO_ALIASES` before `#include <datatype99.h>`.

## Pitfalls

 - `break`/`continue` are prohibited inside statements provided to `of` and `ifLet`; use `goto` labels instead.
 - For the sake of simplicity, pattern matching works as if you were always supplying a modifiable value to `match`, so make sure you do **not** mutate it through bindings introduced by `of`.
 - To accept an array in a variant, you must put it into a separate `struct`; see [`examples/array_in_variant.c`](examples/array_in_variant.c).

## Credits

Thanks to Rust and ML for their implementations of sum types.

## Learning resources

 - [_Unleashing Sum Types in Pure C99_](https://medium.com/@hirrolot/unleashing-sum-types-in-pure-c99-31544302d2ba) by Hirrolot

## FAQ

### Q: Why use C instead of Rust/Zig/whatever else?

A:

 - Datatype99 can be integrated into existing code bases written in pure C.
 - Sometimes C is the only choice.

### Q: Why not third-party code generators?

A: See [Metalang99's README >>](https://github.com/Hirrolot/metalang99#q-why-not-third-party-code-generators).

### Q: How does it work?

A: The `datatype` macro generates a tagged union accompanied with type hints and value constructors. Pattern matching desugars merely to a switch statement. To generate all this stuff, [Metalang99] is used, which is a preprocessor metaprogramming library.

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
