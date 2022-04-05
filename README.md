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
