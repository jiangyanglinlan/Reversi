#-------------------------------------------------
#
# Project created by QtCreator 2017-05-09T11:59:45
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia
CONFIG += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Reversi
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

RESOURCES += \
    images.qrc
