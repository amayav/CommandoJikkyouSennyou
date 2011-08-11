#-------------------------------------------------
#
# Project created by QtCreator 2011-08-02T23:22:56
#
#-------------------------------------------------

QT       += core gui xml network

TARGET = CommandoJikkyouSennyou
TEMPLATE = app


SOURCES += main.cpp\
    widget.cpp \
    extrapushbutton.cpp

HEADERS  += \
    widget.h \
    extrapushbutton.h

FORMS    += \
    widget.ui

CONFIG += kqoauth

unix:!symbian|win32: LIBS += \
    $$PWD/kqoauth-kqoauth/lib/libkqoauthd0.a \
    $$PWD/kqoauth-kqoauth/lib/kqoauthd0.dll \
    $$PWD/qjson-qjson/lib/libqjson0.a \
    $$PWD/qjson-qjson/lib/qjson0.dll \
    $$PWD/openssl/lib/MinGW/ssleay32.a \
    $$PWD/openssl/lib/MinGW/ssleay32.dll \
    $$PWD/openssl/lib/MinGW/libeay32.a \
    $$PWD/openssl/lib/MinGW/libeay32.dll \

INCLUDEPATH += \
    $$PWD/kqoauth-kqoauth/src \
    $$PWD/kqoauth-kqoauth/include \
    $$PWD/qjson-qjson/src \
    $$PWD/openssl/include
DEPENDPATH += \
    $$PWD/kqoauth-kqoauth/src \
    $$PWD/qjson-qjson/src \
    $$PWD/openssl/include
