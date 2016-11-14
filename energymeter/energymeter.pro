QT += charts
QT += core gui widgets
QT += network
QT += dbus
QT += gui

TARGET = energymeter
HEADERS += chart.h
SOURCES += energymeter.cpp chart.cpp chartview.cpp

INCLUDEPATH += ../
