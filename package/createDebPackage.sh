#!/bin/bash
dpkg-deb --build --root-owner-group mdtool_1.0-0_amd64
dpkg-deb --build --root-owner-group mdtool_1.0-0_armhf
dpkg-deb --build --root-owner-group mdtool_1.0-0_aarm64
