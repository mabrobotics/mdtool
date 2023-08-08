#!/usr/bin/env bash

cd ..
mkdir build
cd build
cmake .. -DMAKE_TESTS_MDTOOL=ON
if make -j ; then
    echo "Build successful"
else
    echo "Build failed"
    exit 1
fi
cd test
./mdtool_test