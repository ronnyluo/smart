#-------------------------------------------------
#
# Project created by QtCreator 2015-10-31T12:23:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += network  qml  webkitwidgets

TARGET = tool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    calendaritem.cpp \
    calendar.cpp \
    priceeditor.cpp \
    network.cpp \
    captchadialog.cpp \
    define.cpp \
    dialogwebview.cpp

HEADERS  += mainwindow.h \
    define.h \
    calendaritem.h \
    calendar.h \
    priceeditor.h \
    network.h \
    captchadialog.h \
    dialogwebview.h

FORMS    += mainwindow.ui \
    calendaritem.ui \
    calendar.ui \
    priceeditor.ui \
    captchadialog.ui \
    dialogwebview.ui
