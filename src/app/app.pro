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
    abimoRreader.h \
    abimoRrecord.h \
    bagrov.h \
    calculation.h \
    config.h \
    constants.h \
    counters.h \
    dbaseField.h \
    dbaseFile.h \
    dbaseReader.h \
    dbaseWriter.h \
    effectivenessUunsealed.h \
    helpers.h \
    initVvalues.h \
    main.h \
    mainWwindow.h \
    pdr.h \
    saxHhandler.h

SOURCES += \
    abimoRreader.cpp \
    abimoRrecord.cpp \
    bagrov.cpp \
    calculation.cpp \
    config.cpp \
    counters.cpp \
    dbaseField.cpp \
    dbaseFile.cpp \
    dbaseReader.cpp \
    dbaseWriter.cpp \
    effectivenessUunsealed.cpp \
    helpers.cpp \
    initVvalues.cpp \
    main.cpp \
    mainWwindow.cpp \
    pdr.cpp \
    saxHhandler.cpp

#RC_FILE += AbimoQt.rc
#OTHER_FILES += release/config.xml
#INSTALLS += TARGET
