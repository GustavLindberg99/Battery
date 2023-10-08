QT += widgets
QT += network

CONFIG += C++17

SOURCES += \
    main.cpp \
    updates.cpp

RESOURCES += \
    resource.qrc

TRANSLATIONS = \
    battery_fr.ts\
    battery_sv.ts

win32:RC_FILE = resource.rc

HEADERS += \
    updates.h \
    version.h

