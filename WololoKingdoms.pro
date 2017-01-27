QT += core
QT -= gui

CONFIG += c++11

TARGET = WololoKingdoms
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    fixes/portuguesefix.cpp \
    fixes/demoshipfix.cpp \
    fixes/berbersutfix.cpp \
    fixes/ethiopiansfreepikeupgradefix.cpp \
    fixes/maliansfreeminingupgradefix.cpp \
    fixes/ai900unitidfix.cpp \
    fixes/hotkeysfix.cpp \
    fixes/disablenonworkingunits.cpp \
    fixes/vietfix.cpp \
    fixes/malayfix.cpp \
    fixes/feitoriafix.cpp

win32: LIBS += -L$$PWD/lib/ -llibgenieutils.dll
LIBS += -LD:/local/boost_1_60_0/stage/lib -lboost_system-mgw49-mt-1_60 -lboost_filesystem-mgw49-mt-1_60

win32:RC_ICONS += WololoKingdoms.ico

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

INCLUDEPATH += include/
INCLUDEPATH += D:/local/boost_1_60_0/

HEADERS += \
    include/wololo/Drs.h \
    fixes/portuguesefix.h \
    fixes/demoshipfix.h \
    fixes/berbersutfix.h \
    fixes/ethiopiansfreepikeupgradefix.h \
    fixes/maliansfreeminingupgradefix.h \
    fixes/ai900unitidfix.h \
    fixes/hotkeysfix.h \
    fixes/disablenonworkingunits.h \
    include/wololo/datPatch.h \
    fixes/vietfix.h \
    fixes/malayfix.h \
    fixes/feitoriafix.h

DISTFILES += \
    WololoKingdoms.ico
