QT += core gui widgets

TEMPLATE = lib
CONFIG += shared dll
DEFINES += CORE_LIBRARY

CONFIG += c++17

SOURCES += \
    commonmode.cpp \
    coreplugin.cpp \
    fancytabwidget.cpp \
    icontext.cpp \
    mainwindow.cpp \
    modemanager.cpp \
    navigationwidget.cpp

HEADERS += \
    commonmode.h \
    core_global.h \
    coreplugin.h \
    fancytabwidget.h \
    icontext.h \
    mainwindow.h \
    modemanager.h \
    navigationwidget.h

INCLUDEPATH += $$PWD/../dist/include
LIBS += -L$$PWD/../dist/lib
LIBS += -lPluginSystem

DESTDIR = $${PWD}/../dist/bin/Plugins
