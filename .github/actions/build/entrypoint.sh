#!/bin/bash

set -e -x

env
pwd

export CC="$INPUT_COMPILER"
export CFLAGS="$INPUT_CFLAGS"
export CXXFLAGS="$INPUT_CXXFLAGS"

mkdir _build _install
cd _build
cmake -DCMAKE_INSTALL_PREFIX=../_install ..
make VERBOSE=1
ctest "$INPUT_CTESTFLAGS"
make install

if "${INPUT_COVERAGE}"; then
    bash <(curl -s https://codecov.io/bash)
fi
