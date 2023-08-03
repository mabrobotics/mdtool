#!/usr/bin/env bash

cd ..
mkdir build
cd build
cmake .. -DMAKE_TESTS_MDTOOL=ON
make -j
cd test
./mdtool_test