#!/bin/sh
# Workaround since auto generating doesn't work properly.
uic src/editor/ui/triggereditor.ui -o build/src/editor/ui/ui_triggereditor.h
uic src/editor/ui/modeleditor.ui -o build/src/editor/ui/ui_modeleditor.h
uic src/editor/ui/textureeditor.ui -o build/src/editor/ui/ui_textureeditor.h
uic src/editor/ui/newmapdialog.ui -o build/src/editor/ui/ui_newmapdialog.h
uic src/editor/ui/teamcolordialog.ui -o build/src/editor/ui/ui_teamcolordialog.h
uic src/editor/ui/integerdialog.ui -o build/src/editor/ui/ui_integerdialog.h
uic src/editor/ui/renderstatswidget.ui -o build/src/editor/ui/ui_renderstatswidget.h
uic src/editor/ui/triggertopwidget.ui -o build/src/editor/ui/ui_triggertopwidget.h