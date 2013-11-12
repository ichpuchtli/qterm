#-------------------------------------------------
#
# QTerm (Qt Serial Terminal, with engin.io backend)
#
#-------------------------------------------------

TEMPLATE  = app
TARGET    = qterm

MOC_DIR = bin
OBJECTS_DIR = bin

SOURCES  += main.cpp terminal.cpp
HEADERS  += terminal.h

QT       += core serialport enginio
QT       -= gui

CONFIG   += console

macx-clang {
CONFIG   -= app_bundle
}
