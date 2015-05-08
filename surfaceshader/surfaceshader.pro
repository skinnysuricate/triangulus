QT       -= gui

TARGET = surfaceshader
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
	surfaceshader.cpp \
    light.cpp

HEADERS += \
	surfaceshader.h \
    light.h

DESTDIR = ../lib
OBJECTS_DIR = ../.obj
MOC_DIR = ../.moc
