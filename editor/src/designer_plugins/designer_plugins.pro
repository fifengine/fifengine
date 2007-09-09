# Build plugins for qt designer

CONFIG      += designer plugin debug_and_release
TEMPLATE    = lib
TARGET = fifedit

INCLUDEPATH +=	.. \
		../../../core/src

HEADERS     = ../fpane.h\
              fpaneplugin.h
SOURCES     = ../fpane.cpp\
              fpaneplugin.cpp

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS = target