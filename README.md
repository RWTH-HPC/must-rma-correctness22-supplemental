# On-the-Fly Data Race Detection for MPI RMA Programs with MUST - Supplemental Material
Authors: Simon Schwitanski, Joachim Jenke, Felix Tomski, Christian Terboven, Matthias S. Müller

This is supplemental material for the paper "On-the-Fly Data Race Detection for MPI RMA Programs with MUST".

## Repository Structure
- [must_rma](must_rma/): Sources of MUST-RMA with helper script for installation
- [docker](docker/): Dockerfile to build the software environment for the classification quality benchmarks
- [classification_quality](classification_quality/): Script to generate the classification quality table out of the test cases
- [overhead_measurement](overhead_measurement/): JUBE scripts to reproduce the measurements
- [overhead_results](overhead_results/): Results of the overhead measurements on CLAIX18 (RWTH cluster)

## Source Code
The sources of MUST-RMA are available in [must_rma/src](must_rma/src). Note that the folder contains a bunch of files unrelated to the paper. The contributions / tests can be found in the following folders and files:

- Analysis modules (RMA state tracking, concurrent region analysis)
  - [must_rma/src/modules/OneSidedChecks](must_rma/src/modules/OneSidedChecks)
  - [must_rma/src/specifications/must_onesided_checks.xml](must_rma/src/specifications/must_onesided_checks.xml)
  - [must_rma/src/specifications/must_onesided_checks_api.xml](must_rma/src/specifications/must_onesided_checks_api.xml)
- Own tests
  - [must_rma/src/tests/OneSidedChecks/ProcessLocal](must_rma/src/tests/OneSidedChecks/ProcessLocal): Local buffer races
  - [must_rma/src/tests/OneSidedChecks/AcrossProcesses](must_rma/src/tests/OneSidedChecks/AcrossProcesses): Remote races
- MPI Bugs Initiative tests
  - [must_rma/src/tests/OneSidedChecks/MPIBugsInitiative](must_rma/src/tests/OneSidedChecks/MPIBugsInitiative)


## Software Requirements
The following software packages are needed to reproduce the results:
-   Clang compiler (preferably in version 12.0.1)
-   MPI library with support for at least MPI 3.0 (preferably Intel MPI
    or MPICH)
-   CMake in version 3.20 or newer
-   libxml2 parser (libxml2-dev)
-   Python 3

The classification quality benchmarks in addition need:
-   LLVM lit in version 14.0.0 (available via PyPI)
-   FileCheck binary (distributed with LLVM)

The overhead evaluation in addition needs:
-   JUBE benchmarking environment in version 2.4.2 or newer (<http://www.fz-juelich.de/jsc/jube>)
-   Slurm scheduler to submit the batch scripts

## Classification Quality Benchmarks

To simplify the reproduction of the classification quality benchmarks,
we provide a Dockerfile that provides the required software environment
to build and run MUST-RMA with the benchmarks. If instead a cluster
environment is used, the following Docker build and run steps can be
skipped.

Build the docker image with tag `must-rma`, adjust permissions for the
`must_rma` subfolder to match with the container user, and run the
produced docker image with the MUST source code mounted as volume:

    # cd $ROOT
    # docker build docker -t must-rma
    # chown -R 1000:1000 ./must_rma
    # docker run --rm -it \
        -v $(pwd)/must_rma:/must_rma must-rma /bin/bash

Change to the `must_rma` directory. Install MUST-RMA by using the
provided install script `build_must.sh`:

    $ cd $ROOT/must_rma
    $ ./build_must.sh

Build and installation path can be set within the script. In the
following, we assume that MUST-RMA was built in the folder `$BUILD` and
installed in `$INSTALL`.

Change into the `$BUILD` directory and run the tests:

    $ cd $BUILD
    $ lit -j 1 tests/OneSidedChecks/ | tee test_output.log

This will run all 81 test cases and output the results (number of passed
and failed tests). Passed tests are marked as `PASS`, failed tests with
`FAIL` or `XFAIL`. The number of workers (parameter `-j`) can be
increased, however spawning too many workers might lead to failed test
cases if there are not enough cores available to run the tests.

To produce the result table, we provide a Python script
that parses the `test_output.log` file. Change back to the
`classification_quality` folder and pass the test output log file to the
script:

    $ cd $ROOT/classification_quality
    $ python3 generate_classification_quality_table.py \ 
        $BUILD/test_output.log

To run tests on own applications / binaries, MUST-RMA can be run with:

    $ $INSTALL/bin/mustrun --must:distributed \ 
        --must:tsan --must:rma \ 
        -np <number of processes> <binary> 

## Overhead Evaluation

The overhead evaluation is specific to the CLAIX cluster, so running the
benchmarks in another environment will need manual adaptations. We
provide a JUBE configuration to make reproducibility easier. Important
parameter sets within the JUBE configuration (`prk_rma.xml`) to
consider:

-   `prk_kernel_args_pset`: number of iterations and grid size to be
    used in the kernels
-   `prk_system_pset`: system configuration, e.g., number of nodes to be
    used

After configuring all required parameters, the benchmarks can be run
with

    $ cd $ROOT/overhead_measurement
    $ jube run prk_rma.xml -t kernel_name

where `kernel_name` can be `stencil` or `transpose`.

The JUBE configuration (1) builds MUST-RMA, (2) builds the chosen kernel
with and without TSan instrumentation, (3) submits per requested number
of nodes a Slurm job that runs the three different configurations
(plain, tsan, must-rma). After the Slurm jobs finished, the results can
be retrieved with

    $ cd $ROOT/overhead_measurement
    $ jube result -a bench_run --id <id of JUBE run>

This will print out the results (average iteration time per second per
configuration) as a table.