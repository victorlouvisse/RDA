TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
CONFIG += Wall -O3

SOURCES += main.cpp \
    element.cpp \
    instanceutils.cpp \
    rda.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    element.h \
    instanceutils.h \
    rda.h

