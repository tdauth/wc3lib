#!/bin/bash
# Install all dependencies and build the binaries.
# qtcreator is useful for building and developing wc3lib.
sudo apt install cmake-qt-gui gettext libboost-all-dev libjpeg-dev libbz2-dev qtbase5-dev qtmultimedia5-dev qtcreator libogre-1.12-dev libkf5kdelibs4support-dev extra-cmake-modules doxygen
SOURCE_DIR=$(pwd)
mkdir ../wc3lib-build
cd ../wc3lib-build
cmake "$SOURCE_DIR" -DCMAKE_MODULE_PATH="/usr/share/OGRE/cmake/modules/" -DBLP=ON -DMPQ=ON -DMDLX=ON -DMAP=ON -DJASS=ON -DAPP=ON -DEDITOR=ON -DUSE_KIOSLAVE=ON -DUSE_QBLP=ON -DUSE_OGREBLP=ON -DGAME=ON -DUSE_MIME=ON -DDOC=ON -DCMAKE_BUILD_TYPE=Debug -DWC3_DIR="$HOME/privaterepos/Warcraft III/"
make -j4
