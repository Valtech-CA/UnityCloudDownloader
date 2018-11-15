#-------------------------------------------------
#
# Project created by QtCreator 2018-11-12T15:12:25
#
#-------------------------------------------------

QT       += core gui svg quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UnityCloudDownloader
TEMPLATE = app

win32:RC_FILE = UnityCloudDownloader.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++1z

SOURCES += \
    src/main.cpp \
    src/systemtrayicon.cpp

HEADERS += \
    src/systemtrayicon.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons/icons.qrc \
    views/views.qrc

win32:CONFIG(release, debug|release): LIBS += -lUnityCloudDownloader-Core
else:win32:CONFIG(debug, debug|release): LIBS += -lUnityCloudDownloader-Core
else:unix: LIBS += -L$$OUT_PWD/../UnityCloudDownloader-Core/ -lUnityCloudDownloader-Core

INCLUDEPATH += $$PWD/../UnityCloudDownloader-Core/includes
DEPENDPATH += $$PWD/../UnityCloudDownloader-Core

win32 {
        TEMPDIR  = $$OUT_PWD/tmp/win32/$$TARGET
        MOC_DIR      = $$TEMPDIR
        OBJECTS_DIR  = $$TEMPDIR
        RCC_DIR      = $$TEMPDIR
        UI_DIR       = $$TEMPDIR/Ui
        DEFINES += _WINDOWS WIN32_LEAN_AND_MEAN NOMINMAX
#warning($${TEMPDIR})
}

CONFIG( debug, debug|release ) {
    DESTDIR = $$PWD/../build-debug/
} else {
    DESTDIR = $$PWD/../build/
}
LIBS += -L$$DESTDIR

isEmpty(TARGET_EXT) {
    win32 {
        TARGET_CUSTOM_EXT = .exe
    }
    macx {
        TARGET_CUSTOM_EXT = .app
    }
} else {
    TARGET_CUSTOM_EXT = $${TARGET_EXT}
}

win32 {
    DEPLOY_COMMAND = windeployqt
}
macx {
    DEPLOY_COMMAND = macdeployqt
}

CONFIG( debug, debug|release ) {
    # debug
    DEST_DIR = $$OUT_PWD/debug/
    #DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/debug/$${TARGET}$${TARGET_CUSTOM_EXT}))
    DEPLOY_TARGET = $$shell_quote($$shell_path($${DESTDIR}))
} else {
    # release
    DEST_DIR = $$OUT_PWD/release/
    #DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/release/$${TARGET}$${TARGET_CUSTOM_EXT}))
    DEPLOY_TARGET = $$shell_quote($$shell_path($${DESTDIR}))

    win32 {
        QMAKE_POST_LINK += if defined CERTPWD \"$$PWD/../external/signtool.exe\" sign /fd SHA256 /f \"$$PWD/../code_cert.pfx\" /p \"%CERTPWD%\" /t \"http://timestamp.verisign.com/scripts/timstamp.dll\" \"$$DESTDIR$$TARGET$$TARGET_CUSTOM_EXT\" || ECHO SignTool failed with return code %ERRORLEVEL% $$escape_expand(\n\t)
        warning($$QMAKE_POST_LINK)
    }
}


#QMAKE_POST_LINK += $${DEPLOY_COMMAND} $${DEPLOY_TARGET}
