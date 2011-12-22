#!/bin/bash

if [ "$1"="" ] ; then
	path="build"
else
	path="$1"
fi

echo "Using source tree root path \"$path\"."

# Workaround since auto generating doesn't work properly.
for f in  src/editor/ui/*.ui ; do
	uic "$f" -o "$path"/src/editor/ui/ui_$(basename "$f" .ui).h
done