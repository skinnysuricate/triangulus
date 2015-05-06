QT -= gui

TARGET = triangulatord

CONFIG(release) {
	TARGET = triangulator
	DEFINES += QT_NO_DEBUG_OUTPUT
}

TEMPLATE = lib
CONFIG += staticlib c++11

SOURCES += \
	triangulator.cpp

HEADERS += \
	triangulator.h

DESTDIR = ../lib
OBJECTS_DIR = ../.obj
MOC_DIR = ../.moc
