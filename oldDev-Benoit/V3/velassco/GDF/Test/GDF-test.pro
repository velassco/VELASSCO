#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T15:57:54
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console
CONFIG   -= app_bundle

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GDF-test
TEMPLATE = app


SOURCES += main.cpp \
	server.cpp \
    ../own/gen-cpp/com_constants.cpp \
    ../own/gen-cpp/com_types.cpp \
    ../own/gen-cpp/ServerService.cpp \
    data.cpp

HEADERS  += \
	server.h \
    ../own/gen-cpp/com_constants.h \
    ../own/gen-cpp/com_types.h \
    ../own/gen-cpp/ServerService.h \
    data.h

QMAKE_MAC_SDK = macosx10.9

 LIBS += -L/usr/local/lib -lthrift
 INCLUDEPATH = /usr/local/include /opt/local/include/
