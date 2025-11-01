#!/bin/bash

source ./install-ubuntu-packages.sh

SOURCE_DIR=$(pwd)
mkdir ../../wc3lib-build
cd ../../wc3lib-build
cmake "$SOURCE_DIR" -DCMAKE_MODULE_PATH="/usr/share/OGRE/cmake/modules/" -DBLP=ON -DMPQ=ON -DMDLX=ON -DMAP=ON -DJASS=ON -DAPP=ON -DEDITOR=ON -DUSE_KIOSLAVE=ON -DUSE_QBLP=ON -DUSE_OGREBLP=ON -DGAME=ON -DUSE_MIME=ON -DDOC=ON -DCMAKE_BUILD_TYPE=Debug -DWC3_DIR="$HOME/privaterepos/Warcraft III/"
make -j4
