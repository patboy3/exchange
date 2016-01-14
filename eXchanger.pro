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
INCLUDEPATH += /usr/local/Cellar/openssl/1.0.2e_1/include
LIBS += -L "/usr/local/Cellar/openssl/1.0.2e_1/lib/" -lcrypto -lssl
}


#INCLUDEPATH += C:/OpenSSL-Win32/include
#LIBS += -L"C:/OpenSSL-Win32/lib" -llibeay32

TARGET = eXchanger
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    btcexchange.cpp \
    coinbase.cpp \
    quadriga.cpp

HEADERS  += mainwindow.h \
    btcexchange.h \
    coinbase.h \
    quadriga.h

FORMS    += mainwindow.ui
