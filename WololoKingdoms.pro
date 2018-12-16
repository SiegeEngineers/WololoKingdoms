TARGET = WololoKingdoms
DESTDIR = .
CONFIG += debug_and_release
CONFIG -= console
CONFIG -= app_bundle

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

TEMPLATE = app

# ---------------------------------------------------------
# Main project Config
# ---------------------------------------------------------
QMAKE_CXXFLAGS += -std=c++17
win32: QMAKE_CXXFLAGS += -DUNICODE
SOURCES += main.cpp \
    dialog.cpp \
    mainwindow.cpp \
    paths.cpp
unix: SOURCES += paths_linux.cpp

win32: RC_ICONS += WololoKingdoms.ico

HEADERS += \
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
    paths.h

DISTFILES += \
    WololoKingdoms.ico

FORMS += \
    mainwindow.ui \
    dialog.ui

LIBS += -lstdc++fs
LIBS += -L$$DESTDIR

# ---------------------------------------------------------
# Dependency: libwololokingdoms
# ---------------------------------------------------------
WK_DEST = libwololokingdoms
release: CMAKE_BUILD_TYPE = Release
debug: CMAKE_BUILD_TYPE = Debug

win32 {
  libwololokingdoms.target = $$WK_DEST/libwololokingdoms.dll
  PRE_TARGETDEPS += $$WK_DEST/libwololokingdoms.dll
  QMAKE_PRE_LINK += $$quote(cp $$WK_DEST/third_party/genieutils/libgenieutils.dll $$WK_DEST/libwololokingdoms.dll $$DESTDIR);
} else {
  libwololokingdoms.target = $$WK_DEST/libwololokingdoms.so
  PRE_TARGETDEPS += $$WK_DEST/libwololokingdoms.so
  QMAKE_PRE_LINK += $$quote(cp $$WK_DEST/third_party/genieutils/libgenieutils.so $$WK_DEST/libwololokingdoms.so $$DESTDIR);
}
libwololokingdoms.commands = cmake -B $$WK_DEST -DCMAKE_BUILD_TYPE=$$CMAKE_BUILD_TYPE $$PWD/libwololokingdoms && \
  make -C $$WK_DEST
libwololokingdoms.depends = $$PWD/libwololokingdoms/* \
  $$PWD/libwololokingdoms/fixes/*

QMAKE_EXTRA_TARGETS += libwololokingdoms

LIBS += -lgenieutils
LIBS += -lwololokingdoms

INCLUDEPATH += $$PWD/libwololokingdoms/third_party/genieutils/include
INCLUDEPATH += $$PWD/libwololokingdoms/third_party/pcrio/include
INCLUDEPATH += $$PWD/libwololokingdoms/third_party/genieutils/extern/win-iconv

# ---------------------------------------------------------
# Dependency: Steamworks SDK
# ---------------------------------------------------------

win32: QMAKE_PRE_LINK += $$quote(cp $$PWD/third_party/steam_sdk/redistributable_bin/win64/steam_api.dll $$DESTDIR);
unix: QMAKE_PRE_LINK += $$quote(cp $$PWD/third_party/steam_sdk/redistributable_bin/linux64/libsteam_api.so $$DESTDIR);

LIBS += -lsteam_api

INCLUDEPATH += $$PWD/include
