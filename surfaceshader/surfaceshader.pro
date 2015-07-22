QT -= gui

TARGET = surfaceshader
TEMPLATE = lib
CONFIG += \
	staticlib \
	c++11

SOURCES += \
	triangulator.cpp \
	light.cpp \
	surface.cpp \
	material.cpp \
	distortion.cpp \
	mesh.cpp \
	shaderscene.cpp \
	triangle.cpp \
	cluster.cpp \
	animator.cpp

HEADERS += \
	triangulator.h \
	light.h \
	surface.h \
	material.h \
	distortion.h \
	mesh.h \
	shaderscene.h \
	triangle.h \
	cluster.h \
	animator.h

DESTDIR = ../lib
OBJECTS_DIR = ../.obj
MOC_DIR = ../.moc
