# Load common settings
! include( ../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT += \
    testlib \
    xml

QT -= gui

CONFIG += \
    qt \
    console \
    warn_on \
    depend_includepath \
    testcase

#CONFIG -= app_bundle

#TEMPLATE = app

INCDIR = ../app

#INCLUDEPATH += $$INCDIR

HEADERS += \
    $$INCDIR/abimoreader.h \
    $$INCDIR/abimorecord.h \
    $$INCDIR/bagrov.h \
    $$INCDIR/calculation.h\
    $$INCDIR/config.h\
    $$INCDIR/counters.h\
    $$INCDIR/dbaseField.h \
    $$INCDIR/dbaseReader.h \
    $$INCDIR/dbaseWriter.h \
    $$INCDIR/effectivenessunsealed.h \
    $$INCDIR/helpers.h \
    $$INCDIR/initvalues.h \
    $$INCDIR/pdr.h \
    $$INCDIR/saxhandler.h

SOURCES += \
    $$INCDIR/abimoreader.cpp \
    $$INCDIR/abimorecord.cpp \
    $$INCDIR/bagrov.cpp \
    $$INCDIR/calculation.cpp \
    $$INCDIR/config.cpp \
    $$INCDIR/counters.cpp \
    $$INCDIR/dbaseField.cpp \
    $$INCDIR/dbaseReader.cpp \
    $$INCDIR/dbaseWriter.cpp \
    $$INCDIR/effectivenessunsealed.cpp \
    $$INCDIR/helpers.cpp \
    $$INCDIR/initvalues.cpp \
    $$INCDIR/pdr.cpp \
    $$INCDIR/saxhandler.cpp \
    tst_testabimo.cpp
