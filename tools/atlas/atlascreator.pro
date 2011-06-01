#-------------------------------------------------
#
# Project created by QtCreator 2011-05-02T22:01:15
#
#-------------------------------------------------

QT       += core gui opengl #xml

TARGET = atlascreator
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    image.cpp \
    glwidget.cpp

HEADERS  += mainwindow.h \
    atlas.h \
    image.h \
    glwidget.h \
    stable.h

PRECOMPILED_HEADER = stable.h

FORMS    += mainwindow.ui

INCLUDEPATH += .

win32 {
	LIBS += FreeImage.lib
}

unix {
	LIBS += -lfreeimage
}

# OpenMP
# C++ flags
#QMAKE_CXXFLAGS += -fopenmp
# linker options
#QMAKE_LFLAGS += -fopenmp
