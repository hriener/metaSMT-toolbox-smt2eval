# metaSMT-toolbox-smt2eval

`metaSMT-toolbox-smt2eval` is a `metaSMT` toolbox project that allows
to read and evaluate SMT-LIB2 instances utilizing a varity of
different solving engines.  The projects leverages Z3's parser to read
files in SMT-LIB2 format and `metaSMT` to evaluate them.

## Requirements

The following software is required in order to build `metaSMT-toolbox-smt2eval`

* git
* metaSMT
* Z3

## Build metaSMT-toolbox-smt2eval

`metaSMT-toolbox-smt2eval` is not a standalone application but a
toolbox project for `metaSMT`; thus, the `metaSMT` source code has to
be installed first. The `Z3` libraries are automatically installed as
a part of `metaSMT`.

1. Install `metaSMT` to directory `<metaSMT_DIRECTORY>`.

2. Clone `metaSMT-toolbox-smt2eval` in directory
`<metaSMT_DIRECTORY>/toolbox`.

3. Build `metaSMT` following the normal build instructions. `metaSMT`
will build in the usual way with the additional executables to check
satisfiability and consistency of SMT-LIB2 instances.  The additional
executables follow the naming schemata `smt2_sat_check_*` and
`smt2_consistency_check_*`.
