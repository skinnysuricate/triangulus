QT       -= gui

TARGET = surfaceshader
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
	surfaceshader.cpp

HEADERS += \
	surfaceshader.h

DESTDIR = ../lib
OBJECTS_DIR = ../.obj
MOC_DIR = ../.moc
