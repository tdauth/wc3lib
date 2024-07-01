#!/bin/bash
# Install all dependencies and build the binaries.
# qtcreator is useful for building and developing wc3lib.
sudo apt install cmake-gui gettext libboost-all-dev libjpeg-dev libbz2-dev qt5-default qtmultimedia5-dev qtcreator libogre-1.9-dev libkf5kdelibs4support-dev doxygen
SOURCE_DIR=$(pwd)
mkdir ../wc3lib-build
cd ../wc3lib-build
cmake "$SOURCE_DIR" -DCMAKE_MODULE_PATH="/usr/share/OGRE/cmake/modules/" -DBLP=ON -DMPQ=ON -DMDLX=ON -DMAP=ON -DJASS=ON -DAPP=ON -DEDITOR=ON -DUSE_KIOSLAVE=ON -DUSE_QBLP=ON -DUSE_OGREBLP=ON -DGAME=ON -DUSE_MIME=ON -DDOC=ON -DCMAKE_BUILD_TYPE=Debug -DWC3_DIR="$HOME/privaterepos/Warcraft III/"
make -j4
