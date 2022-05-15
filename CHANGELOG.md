# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## unreleased

### Fixed

 - C++ compilation ([issue #14](https://github.com/Hirrolot/datatype99/issues/14)).

## 1.6.2 - 2022-01-02

### Fixed

 - Initialise `.data.dummy` to `'\0'` for empty variants. Previously, it was uninitialised.

## 1.6.1 - 2021-12-09

### Fixed

 - Specify `C` as the project language in `CMakeLists.txt`. Previously, CMake detected C++ and required a C++ compiler to compile the project.
 - Require CMake v3.11.4.

## 1.6.0 - 2021-12-01

### Added

 - Add the root `CMakeLists.txt` to be able to use CMake with [`FetchContent`] or [`add_subdirectory`].

[`FetchContent`]: https://cmake.org/cmake/help/latest/module/FetchContent.html
[`add_subdirectory`]: https://cmake.org/cmake/help/latest/command/add_subdirectory.html

## 1.5.2 - 2021-11-09

### Fixed

 - The `derive` attribute was made into a macro (see [issue #12](https://github.com/Hirrolot/datatype99/issues/12)).

## 1.5.1 - 2021-10-06

### Fixed

 - Previously, shortcuts referring to functional macros were object-like. To avoid nasty compilation errors, now they are function-like too:
   - `datatype(...)`
   - `record(...)`
   - `match(val)`
   - `MATCHES(val, tag)`
   - `matches(val, tag)`
   - `ifLet(val, tag, ...)`
   - `of(...)`

## 1.5.0 - 2021-08-27

### Added

 - `DATATYPE99_VERSION_COMPATIBLE` to check for a SemVer-compatible version.
 - `DATATYPE99_VERSION_EQ` to check for an exact version.

## 1.4.0 - 2021-08-13

### Added

 - `MATCHES`/`MATCHES99` to know where is a macro and where is a function.

### Deprecated

 - `matches`/`matches99`; use `MATCHES`/`MATCHES99` instead.

## 1.3.0 - 2021-07-27

### Added

 - An empty record -- `record(Foo)`.

## 1.2.0 - 2021-06-06

### Added

 - `record`, `DATATYPE99_record`, `DATATYPE99_RECORD_DERIVE_` to generate a record type.
 - The `dummy` deriver for record types.

### Fixed

 - Add the missing arity specifier for `DATATYPE99_datatype`.
 - Make Datatype99 work on TCC (see [issue #10](https://github.com/Hirrolot/datatype99/issues/10)).

## 1.1.0 - 2021-04-24

### Added

 - A mechanism for deriving: `datatype` now can be specified with `derive(...)`.
 - Manipulation of attributes:
   - `DATATYPE99_attrIsPresent`/`DATATYPE99_ATTR_IS_PRESENT` to check the presence of an attribute.
   - `DATATYPE99_attrValue`/`DATATYPE99_ATTR_VALUE` to extract an attribute's value.
   - `DATATYPE99_assertAttrIsPresent` to assert a mandatory attribute.
 - A deriver `dummy` which generates nothing.
 - Metalang99-compliant counterparts: `DATATYPE99_of`, `DATATYPE99_ifLet`.

### Changed

 - Do **not** suppress `-Wmisleading-indentation` (GCC) automatically in `match`.

### Fixed

 - `#pragma GCC diagnostic` inside functions error on GCC older than 4.6 ([issue #8](https://github.com/Hirrolot/datatype99/issues/8)).
 - Inconsistent suppression of `-Wreturn-type` ([issue #9](https://github.com/Hirrolot/datatype99/issues/9)).
 - Suppress Clang's `-Wcast-qual` in `ifLet`.

## 1.0.0 - 2021-03-28

### Added

 - `DATATYPE99_MAJOR`, `DATATYPE99_MINOR`, `DATATYPE99_PATCH`.

### Changed

 - `Unit` => `UnitT`, `unit` => `unit_v` (less likely to conflict with user-defined identifiers).

### Fixed

 - Suppress Clang's `-Wcast-qual` in `match`.

## 0.4.0 - 2021-03-27

### Added

 - `ifLet` to match only a single variant.

### Changed

 - `<datatype-name>Data` => `<datatype-name>Variants` (less likely to conflict with user-defined types).

### Fixed

 - Avoid using `METALANG99_semicolon` because it turned out to be [dangerous](https://github.com/Hirrolot/metalang99/commit/f17f06adf1a747a8897bbc90c598b2be21c945c8).
 - Clang's `-Wused-but-marked-unused` in `of`.
 - The specification now requires an lvalue for `match`.

## 0.3.1 - 2021-02-28

### Added

 - Generate `<datatype-name>Tag` and `<datatype-name>Data`.

### Fixed

 - Suppress `-Wmisleading-indentation` only when `__GNUC__ >= 6`.
 - Suppress `-Wreturn-type` for Clang too.
 - Make it work on C++11.

## 0.3.0 - 2021-02-26

### Changed

 - Force users to download Metalang99 as well, i.e. don't use it as a Git submodule.
 - Annotate value constructors with `__attribute__((warn_unused_result))` and `__attribute__((const))` if supported.

### Added

 - Generate `typedef struct <datatype-name> <datatype-name>;` beforehand so `<datatype-name> *` can be used instead of `struct <datatype-name> *` inside variants.
 - The reserved `_` identifier for `of`.
 - Error reporting about an unparenthesised variant like `Bar(int)`.
 - `Unit` and `unit`.

## 0.2.0 - 2021-02-05

### Changed

 - Replace `Epilepsy` with `Metalang99` (more neutral).

## 0.1.0 - 2021-02-04

### Added

 - This excellent project.
