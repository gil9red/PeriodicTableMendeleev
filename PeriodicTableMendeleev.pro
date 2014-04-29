QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PeriodicTableMendeleev
TEMPLATE = app

DESTDIR = ../bin

SOURCES += main.cpp \
    PeriodicTableParser.cpp

HEADERS  += \
    PeriodicTableParser.h \
    utils.h

RESOURCES += \
    resource.qrc

OTHER_FILES += \
    resource/periodic_table_mendeleev.xml
