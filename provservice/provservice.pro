TARGET = provisioningservice
TEMPLATE = app
QT = core dbus
QTDIR = /usr/lib/qt5

LIBS += -lwbxml2

HEADERS += provisioningservice.h \
           provisioningservicedbus.h

SOURCES += provisioningservice.cpp
