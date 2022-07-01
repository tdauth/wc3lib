#!/bin/bash

SOURCE_FILEPATH="$(pwd)/AbilityDataNew.w3a"
TARGET_FILEPATH="$(pwd)/AbilityDataNewHotkeys.w3a"
ABILITY_DATA_FILEPATH="$(pwd)/..//share/war3/Units/AbilityData.slk"

cd ../../wc3lib-build/src/app
make

./wc3abilityhotkeyfixer --verbose --overwrite --hotkeys QWER --abilitydata "$ABILITY_DATA_FILEPATH" "$TARGET_FILEPATH" "$SOURCE_FILEPATH"
