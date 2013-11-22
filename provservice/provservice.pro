TARGET = provisioningservice
TEMPLATE = app
QT = core dbus
CONFIG += link_pkgconfig
QTDIR = /usr/lib/qt5

LIBS += -lwbxml2

HEADERS += provisioningservice.h

SOURCES += provisioningservice.cpp
