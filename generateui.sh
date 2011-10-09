#!/bin/bash

if [ "$1"="" ] ; then
	path="build"
else
	path="$1"
fi

echo "Using source tree root path \"$path\"."

# Workaround since auto generating doesn't work properly.
uic src/editor/ui/triggereditor.ui -o "$path"/src/editor/ui/ui_triggereditor.h
uic src/editor/ui/modeleditor.ui -o "$path"/src/editor/ui/ui_modeleditor.h
uic src/editor/ui/textureeditor.ui -o "$path"/src/editor/ui/ui_textureeditor.h
uic src/editor/ui/newmapdialog.ui -o "$path"/src/editor/ui/ui_newmapdialog.h
uic src/editor/ui/teamcolordialog.ui -o "$path"/src/editor/ui/ui_teamcolordialog.h
uic src/editor/ui/integerdialog.ui -o "$path"/src/editor/ui/ui_integerdialog.h
uic src/editor/ui/renderstatswidget.ui -o "$path"/src/editor/ui/ui_renderstatswidget.h
uic src/editor/ui/triggertopwidget.ui -o "$path"/src/editor/ui/ui_triggertopwidget.h
uic src/editor/ui/sourcesdialog.ui -o "$path"/src/editor/ui/ui_sourcesdialog.h