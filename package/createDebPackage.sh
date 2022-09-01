#!/bin/bash
dpkg-deb --build --root-owner-group mdtool_1.1-0_amd64
dpkg-deb --build --root-owner-group mdtool_1.1-0_armhf
dpkg-deb --build --root-owner-group mdtool_1.1-0_aarm64
