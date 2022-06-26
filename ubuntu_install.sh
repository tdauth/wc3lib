#!/bin/bash
# Install all dependencies and build the binaries.
# qtcreator is useful for building and developing wc3lib.
sudo apt install cmake-gui gettext libboost-all-dev libjpeg-dev libbz2-dev qt5-default qtmultimedia5-dev qtcreator
SOURCE_DIR=$(pwd)
mkdir ../wc3lib-build
cd ../wc3lib-build
cmake "$SOURCE_DIR"
make -j4
