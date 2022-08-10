#!/usr/bin/env bash
set -e

## Configuration variables
### MUST repo location
MUST_REPO_DIR=$(pwd)/src/

### Build and install directories
BUILD_DIR=$(pwd)/build/
INSTALL_DIR=$(pwd)/install/

# Use clang compiler
export CC=clang
export CXX=clang++
export MPICH_CC=clang
export MPICH_CXX=clang++

CMAKE_FLAGS="-DCMAKE_INSTALL_PREFIX=$INSTALL_DIR -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_TYPEART=OFF -DENABLE_FORTRAN=OFF -DENABLE_TESTS=ON -DTESTS_WORKERS=1"

echo "= Configuration ="
echo "MUST_REPO_DIR=$MUST_REPO_DIR"
echo "BUILD_DIR=$BUILD_DIR"
echo "INSTALL_DIR=$INSTALL_DIR"
echo "CMAKE_FLAGS=$CMAKE_FLAGS"

# Finally configure and build MUST
mkdir -p $BUILD_DIR
mkdir -p $INSTALL_DIR
cd $BUILD_DIR
cmake $CMAKE_FLAGS $MUST_REPO_DIR
make -j8 install install-prebuilds