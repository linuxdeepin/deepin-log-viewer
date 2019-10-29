QT += core
QT -= gui

CONFIG += c++11

TARGET = logViewerAuth
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
DEFINES += QT_DEPRECATED_WARNINGS

target.path = /usr/bin

INSTALLS += target

