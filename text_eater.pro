#-------------------------------------------------
#
# Project created by QtCreator 2016-04-06T00:21:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++14

TARGET = text_eater
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ObjectMap.cpp \
    ImageEdit.cpp \
    EditAction.cpp \
    ConfigWidget.cpp

HEADERS  += mainwindow.h \
    ObjectMap.h \
    ImageEdit.h \
    EditAction.h \
    ConfigWidget.h

TRANSLATIONS=test.ts
