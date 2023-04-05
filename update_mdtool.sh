#!/usr/bin/env bash
sudo ./release.sh
sudo apt remove mdtool* -y
sudo apt install ./build/packages/mdtool-amd64*

