#!/usr/bin/env bash
TARGETS="amd64 arm64 armhf"
rm -rf build
mkdir -p build
cd build

apt install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi
apt install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
apt install g++-aarch64-linux-gnu

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


