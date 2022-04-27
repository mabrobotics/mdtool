# mdtools
This repo contains tools for configuring md80 drives.

# Building
to build:
```
mkdir build
cd build
cmake ..
make
OR
make install
```
`make` alone is for development, when `make install` is for release - it copies the files to `package/` directory, for easy .deb package creation.

## Cross compilation
The tool can be build using native compilers (as above) or using arm32/arm64 compilers ( for usage with embedded computers, mainly Raspbbery Pi).
### Dependencies
for any cross-compilation
```
sudo apt install gcc make gcc-multilib g++-multilib
```
Additionally for arm32 (armv7l)
```
sudo apt install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi binutils-arm-linux-gnueabi
```
and for arm64 (aarm64)
```
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu
```
### Building
To build for armhf (arm 32-bit) or aarm64 (arm 64*bit):
Clean build directory
```
cd build
rm -r *
```
Prepare Makefile with proper architecure
```
cmake .. -DARCH=armhf
OR
cmake .. -DARCH=aarm64
```
Build
```
make
OR
make install
```
# .deb package
Based on :
https://www.internalpointers.com/post/build-binary-deb-package-practical-guide

Naming conventions:
`<name>_<version>-<revision>_<architecture>.deb`
when changeding package name things that need to be done:
- change package directory name
- change packege version in DEBIAN/control
- change directory names in INSTALL section in the CMakeLists.txt
  
To build repo to package first build with `make install`. Then go to `package/` directory and run
```
./createDebPackage.sh
```
This will create a .deb archive that can be installed with 
```
sudo apt install ./mdtool_xx-x_xx
```
and later uninstalled with 
```
sudo apt remove mdtool
```
