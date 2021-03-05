# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [unreleased]

### Added

 - `ifLet99`

### Fixed

 - Avoid using `METALANG99_semicolon` because it turned out to be [dangerous](https://github.com/Hirrolot/metalang99/commit/f17f06adf1a747a8897bbc90c598b2be21c945c8).
 - Clang's `-Wused-but-marked-unused` in `of99`.

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
 - The reserved `_` identifier for `of99`.
 - Error reporting about an unparenthesised variant like `Bar(int)`.
 - `Unit99` and `unit99`.

## [0.2.0] - 2021-02-05

### Changed

 - Replace `Epilepsy` with `Metalang99` (more neutral).

## [0.1.0] - 2021-02-04

### Added

 - This excellent project.
