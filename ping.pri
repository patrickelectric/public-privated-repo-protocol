SRC_PATH = $$PWD/src

INCLUDEPATH += $$SRC_PATH

HEADERS += \
    $$SRC_PATH/ping.h
SOURCES += \
    $$SRC_PATH/ping.cpp

include($$SRC_PATH/link/link.pri)