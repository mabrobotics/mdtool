# Legacy warning
This is most likely the final release of mdtool, it is now considered Legacy. We will still provide
technical support for candle library and mdtool, guaranteed at least until 2026. 
Further development of the CANdle Ecosystem is going to happen through [CANdle-SDK](https://github.com/mabrobotics/CANdle-SDK)

# MDtool
![example workflow](https://github.com/mabrobotics/mdtool/actions/workflows/build.yaml/badge.svg)

MDtool is a console application for configuring and performing basic diagnostics on MD80-based drives via CANdle. For more information please refer to [MD80 x CANdle Manual](https://www.mabrobotics.pl/servos/#comp-l6v4io99).

# Installing
The easiest way to install the MDtool is to select the appropriate *.deb package from the MDtool GitHub repo 'Releases' page. To install after the download simply call:
```sudo apt install ./mdtool_xx-x_xx.deb```
After the install please make sure the current user is added to the dialout group using the command: 
```sudo adduser <current user> dialout```
if it wasn't, please reboot the PC:
```sudo reboot```
It is also recommended to install the setserial package which allows for higher communication speeds:
```sudo apt install setserial```

# Building
If for some reason you'd like to build mdtool from source:

```
mkdir build
cd build
cmake ..
make
```
for native architecture, or 

```
./release.sh
```
if you want to build for all architectures at once
