# Load common settings
! include( ../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

#TEMPLATE = app
#TARGET = Abimo
#DEPENDPATH += .
#INCLUDEPATH += .
QT += \
    core \
    widgets \
    xml \

#CONFIG += console

HEADERS += \
    bagrov.h \
    calculation.h \
    config.h \
    constants.h \
    dbaseField.h \
    dbaseReader.h \
    dbaseWriter.h \
    helpers.h \
    initvalues.h \
    main.h \
    mainwindow.h \
    pdr.h \
    saxhandler.h

SOURCES += \
    bagrov.cpp \
    calculation.cpp \
    config.cpp \
    dbaseField.cpp \
    dbaseReader.cpp \
    dbaseWriter.cpp \
    helpers.cpp \
    initvalues.cpp \
    main.cpp \
    mainwindow.cpp \
    pdr.cpp \
    saxhandler.cpp

#RC_FILE += AbimoQt.rc
#OTHER_FILES += release/config.xml
#INSTALLS += TARGET
