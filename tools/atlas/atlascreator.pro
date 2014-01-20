#-------------------------------------------------
#
# Project created by QtCreator 2011-05-02T22:01:15
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = atlascreator
TEMPLATE = app
LIBS += -lGLU
SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    objectcreator.cpp

HEADERS  += mainwindow.h \
    atlas.h \
    glwidget.h \
    stable.h \
    objectcreator.h

PRECOMPILED_HEADER = stable.h

FORMS    += mainwindow.ui \
    objectcreator.ui

INCLUDEPATH += .
DEFINES += NOMINMAX

RESOURCES += \
    resources.qrc
