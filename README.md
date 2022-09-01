# MDtool
MDtool is a console application for configuring and performing basic diagnostics on MD80-based drives via CANdle. For more information please refer to [MD80 x CANdle Manual](https://www.mabrobotics.pl/servos).

# Installing
The easiest way to install the MDtool is to select the appropriate *.deb package from the MDtool GitHub repo 'Releases' page. To install after the download simply call:
```sudo apt install ./mdtool_xx-x_xx.deb```

# Building
If for some reason you'd like to build mdtool from source:

```
mkdir build
cd build
cmake ..
make
```
