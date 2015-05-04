QT += core gui widgets

TARGET = app
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += \
	$$quote(../triangulator) \
	$$quote(../surfaceshader)

SOURCES += \
	main.cpp \
	appwindow.cpp

HEADERS += \
	appwindow.h

DESTDIR = ../bin
OBJECTS_DIR = ../.obj
MOC_DIR = ../.moc
LIB_DIR = ../lib

LIBS += \
	-L$$LIB_DIR -ltriangulator \
	-L$$LIB_DIR -lsurfaceshader
