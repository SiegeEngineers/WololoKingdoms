QT += core gui

QMAKE_CXXFLAGS += -std=c++17
win32: QMAKE_CXXFLAGS += -DUNICODE

TARGET = WololoKingdoms
CONFIG -= console
CONFIG -= app_bundle

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

TEMPLATE = app

SOURCES += main.cpp \
    paths.cpp \
    mainwindow.cpp \
    dialog.cpp

win32: {
  LIBS += -L$$PWD/lib/ -L$$PWD/libwololokingdoms/bin/win32 -llibgenieutils.dll -llibwololokingdoms.dll
} else {
  LIBS += -L$$PWD/libwololokingdoms/bin/linux -lgenieutils -lwololokingdoms
}
LIBS += -L$$PWD/lib/ -lsteam_api
LIBS += -lstdc++fs

win32:RC_ICONS += WololoKingdoms.ico

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

INCLUDEPATH += $$PWD/../.
DEPENDPATH += $$PWD/../.

INCLUDEPATH += $$PWD/libwololokingdoms/third_party/genieutils/include
INCLUDEPATH += $$PWD/libwololokingdoms/third_party/pcrio/include
INCLUDEPATH += $$PWD/libwololokingdoms/third_party/genieutils/extern/win-iconv

INCLUDEPATH += include/

HEADERS += \
    paths.h\
    libwololokingdoms/include/wololo/Drs.h \
    libwololokingdoms/fixes/portuguesefix.h \
    libwololokingdoms/fixes/demoshipfix.h \
    libwololokingdoms/fixes/berbersutfix.h \
    libwololokingdoms/fixes/ethiopiansfreepikeupgradefix.h \
    libwololokingdoms/fixes/maliansfreeminingupgradefix.h \
    libwololokingdoms/fixes/ai900unitidfix.h \
    libwololokingdoms/fixes/hotkeysfix.h \
    libwololokingdoms/fixes/disablenonworkingunits.h \
    libwololokingdoms/fixes/vietfix.h \
    libwololokingdoms/include/wololo/datPatch.h \
    libwololokingdoms/fixes/malayfix.h \
    libwololokingdoms/fixes/feitoriafix.h \
    libwololokingdoms/fixes/burmesefix.h \
    libwololokingdoms/fixes/khmerfix.h \
    mainwindow.h \
    dialog.h \
    libwololokingdoms/fixes/smallfixes.h \
    libwololokingdoms/fixes/siegetowerfix.h \
    include/sdk/public/steam/steam_api.h \
    libwololokingdoms/fixes/cuttingfix.h \
    libwololokingdoms/fixes/tricklebuildingfix.h \
    libwololokingdoms/wkconverter.h \
    wkgui.h \
    libwololokingdoms/wksettings.h

DISTFILES += \
    WololoKingdoms.ico

FORMS += \
    mainwindow.ui \
    dialog.ui
