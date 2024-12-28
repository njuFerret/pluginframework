QT += core gui widgets

CONFIG += c++17

SOURCES += main.cpp

INCLUDEPATH += $$PWD/../dist/include
LIBS += -L$$PWD/../dist/lib
LIBS += -lPluginSystem

DESTDIR = $${PWD}/../dist/bin
