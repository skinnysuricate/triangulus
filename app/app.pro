QT += core gui widgets

TARGET = app
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += \
	$$quote(../surfaceshader)

SOURCES += \
	main.cpp \
#	appwindow.cpp \
	canvaswidget.cpp

HEADERS += \
#	appwindow.h \
	canvaswidget.h

DESTDIR = ../bin
OBJECTS_DIR = ../.obj
MOC_DIR = ../.moc
LIB_DIR = ../lib

LIBS += \
	-L$$LIB_DIR -lsurfaceshader

win32 {
	PRE_TARGETDEPS += $$LIB_DIR/surfaceshader.lib
}

unix {
	CONFIG += c++11
	QMAKE_CXXFLAGS += -std=c++11
	PRE_TARGETDEPS += $$LIB_DIR/libsurfaceshader.a
}
