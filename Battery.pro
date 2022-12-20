QT += widgets

CONFIG += C++11

SOURCES += main.cpp

RESOURCES += \
    resource.qrc

TRANSLATIONS = \
    battery_fr.ts\
    battery_sv.ts

win32:RC_FILE = resource.rc

HEADERS += \
    version.h

