QT += core gui widgets

TEMPLATE = lib
CONFIG += shared dll
DEFINES += HELLOWORLD_LIBRARY

CONFIG += c++17

SOURCES +=  helloworld.cpp painttributton.cpp

HEADERS +=  helloworld.h    painttributton.h

INCLUDEPATH += $$PWD/../dist/include
LIBS += -L$$PWD/../dist/lib
LIBS += -lPluginSystem

DESTDIR = $${PWD}/../dist/bin/Plugins
!exists($$DESTDIR){
  mkpath($$DESTDIR)
}
