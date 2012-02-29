#-------------------------------------------------
#
# Project created by QtCreator 2011-08-02T23:22:56
#
#-------------------------------------------------
# CommandoJikkyouSennyou - Commando Jikkyou Sennyou Client for twitter for Qt.
#
# Author: amayav (vamayav@yahoo.co.jp)
#
#
#  CommandoJikkyouSennyou is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  CommandoJikkyouSennyou is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with CommandoJikkyouSennyou.  If not, see <http://www.gnu.org/licenses/>.
#
QT       += core gui network

TARGET = CommandoJikkyouSennyou
TEMPLATE = app
RC_FILE = icon.rc
OBJECTS_DIR = temp
MOC_DIR = temp

SOURCES += main.cpp\
    widget.cpp \
    extrapushbutton.cpp \
    widgetdomain.cpp \
    latesttweetsfortime.cpp \
    kqoauth-kqoauth/src/kqoauthutils.cpp \
    kqoauth-kqoauth/src/kqoauthrequest_xauth.cpp \
    kqoauth-kqoauth/src/kqoauthrequest_1.cpp \
    kqoauth-kqoauth/src/kqoauthrequest.cpp \
    kqoauth-kqoauth/src/kqoauthmanager.cpp \
    kqoauth-kqoauth/src/kqoauthauthreplyserver.cpp

HEADERS  += \
    widget.h \
    extrapushbutton.h \
    picojson.h \
    widgetdomain.h \
    latesttweetsfortime.h \
    kqoauth-kqoauth/src/kqoauthutils.h \
    kqoauth-kqoauth/src/kqoauthrequest_xauth_p.h \
    kqoauth-kqoauth/src/kqoauthrequest_xauth.h \
    kqoauth-kqoauth/src/kqoauthrequest_p.h \
    kqoauth-kqoauth/src/kqoauthrequest_1.h \
    kqoauth-kqoauth/src/kqoauthrequest.h \
    kqoauth-kqoauth/src/kqoauthmanager_p.h \
    kqoauth-kqoauth/src/kqoauthmanager.h \
    kqoauth-kqoauth/src/kqoauthglobals.h \
    kqoauth-kqoauth/src/kqoauthauthreplyserver_p.h \
    kqoauth-kqoauth/src/kqoauthauthreplyserver.h

FORMS    += \
    widget.ui

CONFIG += kqoauth

unix:!symbian|win32: LIBS += \
    $$PWD/kqoauth-kqoauth/lib/libkqoauth0.a \
    $$PWD/kqoauth-kqoauth/libd/libkqoauthd0.a

INCLUDEPATH += \
    $$PWD/kqoauth-kqoauth/src \
    $$PWD/kqoauth-kqoauth/include
DEPENDPATH += \
    $$PWD/kqoauth-kqoauth/src \
    $$PWD/kqoauth-kqoauth/include

OTHER_FILES += \
    kqoauth-kqoauth/src/src.pro \
    kqoauth-kqoauth/src/kqoauthd_resource.rc











