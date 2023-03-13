
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

DEFINES += DEBUG
#error("QT_TESTCASE_BUILDDIR: $$QT_TESTCASE_BUILDDIR")

QT_TESTCASE_BUILDDIR = $$PWD/my-abimo-test/build
#error("QT_TESTCASE_BUILDDIR: $$QT_TESTCASE_BUILDDIR")

#TEMPLATE = app

INCDIR = ../app

#INCLUDEPATH += $$INCDIR

HEADERS += \
    $$INCDIR/abimoReader.h \
    $$INCDIR/abimoWriter.h \
    $$INCDIR/abimoInputRecord.h \
    $$INCDIR/abimoOutputRecord.h \
    $$INCDIR/bagrov.h \
    $$INCDIR/calculation.h\
    $$INCDIR/config.h\
    $$INCDIR/counters.h\
    $$INCDIR/dbaseField.h \
    $$INCDIR/dbaseFile.h \
    $$INCDIR/dbaseReader.h \
    $$INCDIR/dbaseWriter.h \
    $$INCDIR/effectivenessUnsealed.h \
    $$INCDIR/helpers.h \
    $$INCDIR/initValues.h \
    $$INCDIR/intermediateResults.h \
    $$INCDIR/pdr.h \
    $$INCDIR/saxHandler.h

SOURCES += \
    $$INCDIR/abimoReader.cpp \
    $$INCDIR/abimoWriter.cpp \
    $$INCDIR/abimoInputRecord.cpp \
    $$INCDIR/abimoOutputRecord.cpp \
    $$INCDIR/bagrov.cpp \
    $$INCDIR/calculation.cpp \
    $$INCDIR/config.cpp \
    $$INCDIR/counters.cpp \
    $$INCDIR/dbaseField.cpp \
    $$INCDIR/dbaseFile.cpp \
    $$INCDIR/dbaseReader.cpp \
    $$INCDIR/dbaseWriter.cpp \
    $$INCDIR/effectivenessUnsealed.cpp \
    $$INCDIR/helpers.cpp \
    $$INCDIR/initValues.cpp \
    $$INCDIR/intermediateResults.cpp \
    $$INCDIR/pdr.cpp \
    $$INCDIR/saxHandler.cpp \
    tst_testabimo.cpp
