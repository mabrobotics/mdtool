TARGETS="amd64 aarm64 armhf"
rm -rf build
mkdir -p build
cd build

for TARGET in ${TARGETS}
do    
    # create one build dir per target architecture
    mkdir -p ${TARGET}
    cd ${TARGET}
    cmake -DARCH=${TARGET} ../..
    make -j
    cpack -G DEB
    cd - 
done