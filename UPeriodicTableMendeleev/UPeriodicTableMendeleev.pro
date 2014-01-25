QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PeriodicTableMendeleev
TEMPLATE = app

DESTDIR = ../bin

SOURCES += main.cpp \
    UPeriodicTableParser.cpp

HEADERS  += \
    UPeriodicTableParser.h

RESOURCES += \
    resource.qrc
