#-------------------------------------------------
#
# Project created by QtCreator 2018-11-12T15:16:49
#
#-------------------------------------------------

QT       += network sql

QT       -= gui

TARGET = UnityCloudDownloader-Core
TEMPLATE = lib

DEFINES += UNITYCLOUDDOWNLOADERCORE_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += includes

SOURCES += \
    src/profile.cpp \
    src/buildtarget.cpp \
    src/project.cpp \
    src/unityclouddownloadercore.cpp

HEADERS += \
    includes/unityclouddownloader-core_global.h \
    includes/profile.h \
    includes/project.h \
    includes/buildtarget.h \
    includes/unityclouddownloadercore.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
