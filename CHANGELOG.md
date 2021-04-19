# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [unreleased]

### Added

 - A mechanism for deriving: `datatype` now can be specified with `derive(...)`.
 - `DATATYPE99_ATTR_IS_PRESENT` and `DATATYPE99_ATTR_VALUE` to manipulate variant-specific attributes.
 - A deriver `dummy` which generates nothing.

### Changed

 - Do **not** suppress `-Wmisleading-indentation` (GCC) automatically in `match`.

### Fixed

 - `#pragma GCC diagnostic` inside functions error on GCC older than 4.6 ([issue 8](https://github.com/Hirrolot/datatype99/issues/8)).
 - Inconsistent suppression of `-Wreturn-type` ([issue 9](https://github.com/Hirrolot/datatype99/issues/9)).
 - Suppress Clang's `-Wcast-qual` in `ifLet`.

## [1.0.0] - 2021-03-28

### Added

 - `DATATYPE99_MAJOR`, `DATATYPE99_MINOR`, `DATATYPE99_PATCH`

### Changed

 - `Unit` -> `UnitT`, `unit` -> `unit_v` (less likely to conflict with user-defined identifiers)

### Fixed

 - Suppress Clang's `-Wcast-qual` in `match`.

## [0.4.0] - 2021-03-27

### Added

 - `ifLet`

### Changed

 - `<datatype-name>Data` -> `<datatype-name>Variants` (less likely to conflict with user-defined types)

### Fixed

 - Avoid using `METALANG99_semicolon` because it turned out to be [dangerous](https://github.com/Hirrolot/metalang99/commit/f17f06adf1a747a8897bbc90c598b2be21c945c8).
 - Clang's `-Wused-but-marked-unused` in `of`.
 - The specification now requires an lvalue for `match`.

## [0.3.1] - 2021-02-28

### Added

 - Generate `<datatype-name>Tag` and `<datatype-name>Data`.

### Fixed

 - Suppress `-Wmisleading-indentation` only when `__GNUC__ >= 6`.
 - Suppress `-Wreturn-type` for Clang too.
 - Make it work on C++11.

## [0.3.0] - 2021-02-26

### Changed

 - Force users to download Metalang99 as well, i.e. don't use it as a Git submodule.
 - Annotate value constructors with `__attribute__((warn_unused_result))` and `__attribute__((const))` if supported.

### Added

 - Generate `typedef struct <datatype-name> <datatype-name>;` beforehand so `<datatype-name> *` can be used instead of `struct <datatype-name> *` inside variants.
 - The reserved `_` identifier for `of`.
 - Error reporting about an unparenthesised variant like `Bar(int)`.
 - `Unit` and `unit`.

## [0.2.0] - 2021-02-05

### Changed

 - Replace `Epilepsy` with `Metalang99` (more neutral).

## [0.1.0] - 2021-02-04

### Added

 - This excellent project.
