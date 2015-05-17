QT -= gui

TARGET = surfaceshader
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
	light.cpp \
	surface.cpp \
	material.cpp \
	distortion.cpp \
	geometry.cpp \
	shaderscene.cpp

HEADERS += \
	light.h \
	surface.h \
	material.h \
	distortion.h \
	geometry.h \
	shaderscene.h

DESTDIR = ../lib
OBJECTS_DIR = ../.obj
MOC_DIR = ../.moc
