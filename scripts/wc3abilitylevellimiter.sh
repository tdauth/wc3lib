#!/bin/bash

SOURCE_FILEPATH="$(pwd)/AbilityData.w3a"
TARGET_FILEPATH="$(pwd)/AbilityDataNew.w3a"

cd ../../wc3lib-build/src/app
make

./wc3objectlevellimiter --overwrite -l 15 "$TARGET_FILEPATH" "$SOURCE_FILEPATH"
