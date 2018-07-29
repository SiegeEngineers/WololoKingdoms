QT += core gui

CONFIG += c++11

TARGET = WololoKingdoms
CONFIG -= console
CONFIG -= app_bundle

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

TEMPLATE = app

SOURCES += main.cpp \
    paths.cpp \
    conversions.cpp \
    fixes/portuguesefix.cpp \
    fixes/demoshipfix.cpp \
    fixes/berbersutfix.cpp \
    fixes/ethiopiansfreepikeupgradefix.cpp \
    fixes/maliansfreeminingupgradefix.cpp \
    fixes/ai900unitidfix.cpp \
    fixes/hotkeysfix.cpp \
    fixes/disablenonworkingunits.cpp \
    fixes/malayfix.cpp \
    fixes/feitoriafix.cpp \
    fixes/burmesefix.cpp \
    fixes/incafix.cpp \
    fixes/khmerfix.cpp \
    fixes/vietfix.cpp \
    mainwindow.cpp \
    dialog.cpp \
    fixes/smallfixes.cpp \
    fixes/siegetowerfix.cpp \
    fixes/cuttingfix.cpp \
    fixes/tricklebuildingfix.cpp \
    wkconverter.cpp

win32: LIBS += -L$$PWD/lib/ -llibgenieutils.dll
LIBS += -L$$PWD/lib/ -lsteam_api
LIBS += -LD:/boost_1_63_0/stage/lib -lboost_system-mgw53-mt-1_63 -lboost_filesystem-mgw53-mt-1_63
INCLUDEPATH += C:\GnuWin32\src\zlib-1.2.3
LIBS += -LC:\GnuWin32\src\zlib-1.2.3 -lz
INCLUDEPATH += C:\quazip-0.7.3\quazip
LIBS += -LC:\quazip-0.7.3\quazip\release -lquazip

win32:RC_ICONS += WololoKingdoms.ico

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

INCLUDEPATH += $$PWD/../.
DEPENDPATH += $$PWD/../.

INCLUDEPATH += include/
INCLUDEPATH += D:/boost_1_63_0/

HEADERS += \
    paths.h\
    conversions.h\
    include/wololo/Drs.h \
    fixes/portuguesefix.h \
    fixes/demoshipfix.h \
    fixes/berbersutfix.h \
    fixes/ethiopiansfreepikeupgradefix.h \
    fixes/maliansfreeminingupgradefix.h \
    fixes/ai900unitidfix.h \
    fixes/hotkeysfix.h \
    fixes/disablenonworkingunits.h \
    fixes/vietfix.h \
    include/wololo/datPatch.h \
    fixes/malayfix.h \
    fixes/feitoriafix.h \
    fixes/burmesefix.h \
    fixes/incafix.h \
    fixes/khmerfix.h \
    mainwindow.h \
    dialog.h \
    fixes/smallfixes.h \
    fixes/siegetowerfix.h \
    include/sdk/public/steam/steam_api.h \
    fixes/cuttingfix.h \
    fixes/tricklebuildingfix.h \
    wkconverter.h \
    wkgui.h \
    wksettings.h

DISTFILES += \
    WololoKingdoms.ico

FORMS += \
    mainwindow.ui \
    dialog.ui
