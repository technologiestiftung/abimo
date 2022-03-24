QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += tst_testabimo.cpp

INCDIR = ../app

INCLUDEPATH += $$INCDIR

HEADERS += \
    $$INCDIR/dbaseField.h \
    $$INCDIR/dbaseReader.h \
    $$INCDIR/helpers.h

SOURCES += \
    $$INCDIR/dbaseField.cpp \
    $$INCDIR/dbaseReader.cpp \
    $$INCDIR/helpers.cpp

SOURCES += tst_testabimo.cpp
