QT += core gui

win32 {
  libwololokingdoms.target = lib/libwololokingdoms.dll
  libwololokingdoms.commands = make -C $$PWD/libwololokingdoms win32 && \
    cp $$PWD/libwololokingdoms/bin/win32/libwololokingdoms.dll lib && \
    cp $$PWD/libwololokingdoms/bin/win32/libgenieutils.dll lib
} else {
  libwololokingdoms.target = lib/libwololokingdoms.so
  libwololokingdoms.commands = make -C $$PWD/libwololokingdoms linux && \
    cp $$PWD/libwololokingdoms/bin/linux/libwololokingdoms.so lib && \
    cp $$PWD/libwololokingdoms/bin/linux/libgenieutils.so lib
}

QMAKE_EXTRA_TARGETS += libwololokingdoms
PRE_TARGETDEPS += lib/libwololokingdoms.so

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

unix: SOURCES += paths_linux.cpp

LIBS += -L$$PWD/lib/
LIBS += -lgenieutils -lwololokingdoms
LIBS += -lsteam_api
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
    include/sdk/public/steam/steam_api.h \
    libwololokingdoms/fixes/ai900unitidfix.h \
    libwololokingdoms/fixes/berbersutfix.h \
    libwololokingdoms/fixes/burmesefix.h \
    libwololokingdoms/fixes/cuttingfix.h \
    libwololokingdoms/fixes/demoshipfix.h \
    libwololokingdoms/fixes/disablenonworkingunits.h \
    libwololokingdoms/fixes/ethiopiansfreepikeupgradefix.h \
    libwololokingdoms/fixes/feitoriafix.h \
    libwololokingdoms/fixes/hotkeysfix.h \
    libwololokingdoms/fixes/khmerfix.h \
    libwololokingdoms/fixes/malayfix.h \
    libwololokingdoms/fixes/maliansfreeminingupgradefix.h \
    libwololokingdoms/fixes/portuguesefix.h \
    libwololokingdoms/fixes/siegetowerfix.h \
    libwololokingdoms/fixes/smallfixes.h \
    libwololokingdoms/fixes/tricklebuildingfix.h \
    libwololokingdoms/fixes/vietfix.h \
    libwololokingdoms/include/wololo/datPatch.h \
    libwololokingdoms/base64.h \
    libwololokingdoms/caseless.h \
    libwololokingdoms/drs.h \
    libwololokingdoms/md5.h \
    libwololokingdoms/platform.h \
    libwololokingdoms/string_helpers.h \
    libwololokingdoms/wkconverter.h \
    libwololokingdoms/wksettings.h \
    dialog.h \
    mainwindow.h \
    paths.h\
    wkgui.h

DISTFILES += \
    WololoKingdoms.ico

FORMS += \
    mainwindow.ui \
    dialog.ui
