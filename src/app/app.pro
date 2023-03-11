# Load common settings
! include( ../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

#TEMPLATE = app
TARGET = Abimo
#DEPENDPATH += .
#INCLUDEPATH += .
QT += \
    core \
    widgets \
    xml \

#CONFIG += console

# https://stackoverflow.com/a/4563512
CONFIG *= debug_and_release debug_and_release_target

HEADERS += \
    abimoInputRecord.h \
    abimoOutputRecord.h \
    abimoReader.h \
    bagrov.h \
    calculation.h \
    config.h \
    constants.h \
    counters.h \
    dbaseField.h \
    dbaseFile.h \
    dbaseReader.h \
    dbaseWriter.h \
    effectivenessUnsealed.h \
    helpers.h \
    initValues.h \
    intermediateResults.h \
    main.h \
    mainWindow.h \
    pdr.h \
    saxHandler.h

SOURCES += \
    abimoInputRecord.cpp \
    abimoOutputRecord.cpp \
    abimoReader.cpp \
    bagrov.cpp \
    calculation.cpp \
    config.cpp \
    counters.cpp \
    dbaseField.cpp \
    dbaseFile.cpp \
    dbaseReader.cpp \
    dbaseWriter.cpp \
    effectivenessUnsealed.cpp \
    helpers.cpp \
    initValues.cpp \
    intermediateResults.cpp \
    main.cpp \
    mainWindow.cpp \
    pdr.cpp \
    saxHandler.cpp

#RC_FILE += AbimoQt.rc
#OTHER_FILES += release/config.xml
#INSTALLS += TARGET
