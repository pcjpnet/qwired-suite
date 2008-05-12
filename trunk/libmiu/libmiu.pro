TEMPLATE = lib

CONFIG += staticlib

VERSION = 0.1.0

QT -= gui

QT += network

TARGET = miu

DESTDIR = ../bin/

SOURCES = MiuTransaction.cpp \
	MiuSocket.cpp \
	MiuClient.cpp

HEADERS = MiuTransaction.h \
	MiuSocket.h \
	MiuClient.h


OBJECTS_DIR = ../temp
MOC_DIR = ../temp
QMAKE_CXX = colorgcc
QMAKE_CC = colorgcc
