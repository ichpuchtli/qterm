#-------------------------------------------------
#
# QTerm (Qt Serial Terminal, with engin.io backend)
#
#-------------------------------------------------

TEMPLATE  = app
TARGET    = qterm

MOC_DIR = bin
OBJECTS_DIR = bin

SOURCES  += main.cpp terminal.cpp tagdbadapter.cpp
HEADERS  += terminal.h tagdbadapter.h

OTHER_FILES += qterm.ini

QT       += core serialport enginio sql
QT       -= gui

CONFIG   += console

macx-clang {
CONFIG   -= app_bundle
}
