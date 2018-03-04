CONFIG += c++11

SRC_PATH = $$PWD/src

INCLUDEPATH += $$SRC_PATH

HEADERS += \
    $$SRC_PATH/ping.h \
    $$PWD/src/sensor.h \
    $$PWD/src/sensorarbitrary.h \
    $$PWD/src/wraith32.h
SOURCES += \
    $$SRC_PATH/ping.cpp \
    $$PWD/src/sensor.cpp \
    $$PWD/src/sensorarbitrary.cpp

include($$SRC_PATH/link/link.pri)
include($$SRC_PATH/protocol/protocol.pri)