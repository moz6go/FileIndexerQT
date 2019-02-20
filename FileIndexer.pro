#-------------------------------------------------
#
# Project created by QtCreator 2019-01-23T13:25:34
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileIndexer
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    controller.cpp \
    statechecker.cpp \
    indexer.cpp 

HEADERS  += mainwindow.h \
    controller.h \
    statechecker.h \
    indexer.h \
    general.h 

FORMS    += mainwindow.ui

RESOURCES += \
    img.qrc
