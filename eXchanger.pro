#-------------------------------------------------
#
# Project created by QtCreator 2016-01-02T19:16:02
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


macx{
#PKGCONFIG += openssl
#ICON = qrc/imgs/icon.icns
#INCLUDEPATH += $$PWD/includes/OsX/openssl/include
#LIBS += -L "$$PWD/includes/OsX/openssl/lib/" -lcrypto -lssl
}

win32{
#INCLUDEPATH += $$PWD/includes/WIN/openssl/include
#LIBS += -L"$$PWD/includes/WIN/openssl/lib" -llibeay32
}


TARGET = eXchanger
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    btcexchange.cpp \
    coinbase.cpp \
    quadriga.cpp \
    trade.cpp \
    threadorderbook.cpp \
    kraken.cpp \
    poloniex.cpp

HEADERS  += mainwindow.h \
    btcexchange.h \
    coinbase.h \
    quadriga.h \
    trade.h \
    threadorderbook.h \
    kraken.h \
    poloniex.h

FORMS    += mainwindow.ui
