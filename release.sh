#!/usr/bin/env bash
TARGETS="amd64 arm64 armhf"
rm -rf build
mkdir -p build
cd build

mkdir packages

for TARGET in ${TARGETS}
do    
    # create one build dir per target architecture
    mkdir -p ${TARGET}
    cd ${TARGET}
    cmake -DARCH=${TARGET} ../..
    make -j
    cpack -G DEB
    cp *.deb ../packages
    cd - 
done


