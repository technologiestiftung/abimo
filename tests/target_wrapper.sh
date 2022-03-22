#!/bin/sh
PATH=/C/Qt/Qt5.12.12/5.12.12/mingw73_64/bin:$PATH
export PATH
QT_PLUGIN_PATH=/C/Qt/Qt5.12.12/5.12.12/mingw73_64/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}
export QT_PLUGIN_PATH
exec "$@"
