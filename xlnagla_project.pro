QT       -= core

QT       -= gui

TARGET = utils
CONFIG   -= console
CONFIG   -= app_bundle

QMAKE_CXX = g++-4.6
QMAKE_LINK = g++-4.6
QMAKE_CXXFLAGS += -std=c++0x -g3


HEADERS+=\
assert_once.h \
future.h \
HiddenMarkovModel.h \
oxthread.h

SOURCES += \
    main.cpp

