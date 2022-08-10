# MUST Test Suite

MUST uses [lit](https://www.llvm.org/docs/CommandGuide/lit.html) and
[FileCheck](https://www.llvm.org/docs/CommandGuide/FileCheck.html) for its test
suite.

## Prequisites

- An installation of LLVM's FileCheck (Tested with version 11). Unfortunately
  FileCheck is deeply integrated into the LLVM suite and is not distributed
  separately.

  Some examples how to get an installation of FileCheck:
  - On CLAIX: `load module clang`
  - On Fedora 35: `dnf install llvm`
  - On Ubuntu 20.04: `apt install --install-recommends llvm`

  If you are not using a system package you have to build it
  yourself as part of the LLVM suite. This method might also require to point
  to the executables manually as described in the section
  [Test configuration](#test-configuration).

- The lit python package in version 12.0.0 at least. Version 14.0.0 is required
  if you also want to run unit tests. You can install lit using pip:
  ```sh
  pip3 install 'lit~=14.0'
  ```
- Python3 (Tested with versions 3.9 and 3.10)

## Quickstart

You should set the CMake option `ENABLE_TESTS` to `ON` first. Then execute the
CMake `check` target to run the whole test suite (with a single worker by default):

```sh
cd ${BUILDDIR}
make
make install install-prebuilds
make check
```

The test results are then printed out on the console and additionally saved in
JSON format to `${BUILDDIR}/tests/check.out.json`.

## Test configuration

The test execution can be configured using the following CMake variables:

- `ENABLE_TESTS`: Highly recommendend. Enables building of tests. (Default: OFF)
- `TESTS_WORKERS`: Sets the amount of tests that may run in parallel. Beware:
  The tests _spawn more than one_ task each! Setting this to the processor count
  might stall the system. (Default: 1)
- The test suite expects the lit and FileCheck executables to be in the system
  path. There are several options how to specify which executables of lit and
  FileCheck to use:
  1. Set the environment variable 'LLVM_ROOT' to the installation prefix of the
     installed LLVM suite. On CLAIX this can be done with `module load clang`.
  2. `LLVM_TOOLS_BINARY_DIR`: The `bin` directory of the LLVM installation.
  3. `LLVM_LIT_PATH` & `LLVM_FILECHECK_PATH`: The direct paths to the lit or
     FileCheck executables. Usually set by CMake if it could find those. (Use
	 also to override the executable found by CMake. E.g. to use another one
	 installed with `pip3`.)
- `TESTS_DISABLE_MUST_CLEAN`: Selects whether the mustrun option `--must:clean`
  is disabled during test runs. (Default: False)
- `TEST_PREBUILDS`: Selects whether the prebuilds for tests should be installed.
  Accelerates test execution but increases installation time. This is only
  available if `ENABLE_PREBUILD` is true.  (Default: True)
- `MUST_FAST_TESTS_ONLY`: Skip tests that are known to be slow. (Default: False)

## Running the tests

### Before running tests

Test build is disabled by default. Enable it by setting the CMake variable
`ENABLE_TESTS` to `ON`.

The tests can currently only be run with MUST already installed, so you should
execute the `install` and `install-prebuilds` targets before running them:

```sh
make install install-prebuilds
```

### Test targets

MUST's build system provides some targets for convenient test execution.

The target `check` runs all tests. Its output is saved in JSON format in the
build directory at `${BUILDDIR}/tests/check.out.json`.

The targets `check-<subdir>` correspond to each subdirectory `<subdir>` in the
`tests` directory, i.e `make check-DeadlockDetection` only runs the tests found
in `tests/DeadlockDetection`.

To run all but slow tests there is also a `check-fast` target.

Each test target stores its results in JSON format in the build directory at
`${BUILDDIR}/tests/<targetname>.out.json`.

### Modify test execution

You can add and override the options passed to lit or FileCheck using the
environment variables `LIT_OPTS` and `FILECHECK_OPTS`. This allows to quickly
change the output verbosity or amount of parallel workers.

### Skip slow tests

Some tests (e.g. those that deadlock) are slow to execute. You can skip their
execution by setting the environment variable `MUST_FAST_TESTS_ONLY` before
running the test suite or alternatively by setting the cmake option
`MUST_FAST_TESTS_ONLY` to true at configure time.

MUST also provides the convenience target `check-fast`, that executes `check`
skipping slow tests. This target is always available regardless of the
`MUST_FAST_TESTS_ONLY` setting.

See [below](#mark-slow-tests) on how to mark a test as slow.

## Writing tests

All MUST tests are written with with the same test pattern: Each C/C++/Fortran
source file in the suite is a test. The comments that start with `RUN` get
expanded by applying the substitutions defined in lit and the lit
configuration. The expanded `RUN` lines get executed similar to shell commands
(inclusive piping and redirects).

Currently all tests execute MUST on the compiled test source and pipe MUSTS
console output to FileCheck. FileCheck then compares the piped text with the
`CHECK` directives that are given as comments in the test source. The LLVM
project provides [a comprehensive guide](https://www.llvm.org/docs/CommandGuide/FileCheck.html#tutorial)
for FileCheck.

It is also a good idea to look into the [manual of lit](https://www.llvm.org/docs/CommandGuide/lit.html).
(Although it is not always up to date with the latest version. Use `lit --help`
as definite source of truth for available options.)

### Available lit substitutions and features

To get an overview of the available substitutions and features run:

```sh
LIT_OPTS="--show-suites" make check
```

Note, that this list does not contain the
[substitutions provided by lit](https://www.llvm.org/docs/CommandGuide/lit.html#substitutions).

### Mark slow tests

MUST's lit.cfg defines the lit feature "fast-tests". Tests are marked manually
as slow (usually those that take >20s) by adding the line
```c
// UNSUPPORTED: fast-tests
```
to their test sources.
