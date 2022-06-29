#!/bin/bash

SOURCE_FILEPATH="$(pwd)/war3map.w3d"
TARGET_FILEPATH="$(pwd)/war3mapNew.w3d"

cd ../../wc3lib-build/src/app
make

./wc3baseobjectchanger --s "ASbl" --t "OOal" "$TARGET_FILEPATH" "$SOURCE_FILEPATH"
