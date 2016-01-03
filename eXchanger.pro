#-------------------------------------------------
#
# Project created by QtCreator 2016-01-02T19:16:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = eXchanger
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    btcexchange.cpp \
    cexio.cpp \
    test.cpp

HEADERS  += mainwindow.h \
    btcexchange.h \
    cexio.h \
    test.h

FORMS    += mainwindow.ui
