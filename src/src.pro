QT += core gui dtkwidget dtkgui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dde-log-viewer
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
#PKGCONFIG +=

LIBS += -L /usr/lib

include(../thirdlib/QtXlsxWriter/src/xlsx/qtxlsx.pri)
include(../thirdlib/docx/docx.pri)

SOURCES += \
        main.cpp \
    filtercontent.cpp \
    displaycontent.cpp \
    logcollectormain.cpp \
    logfileparser.cpp \
    logtreeview.cpp \
    journalwork.cpp \
    logpasswordauth.cpp \
    logexportwidget.cpp \
    utils.cpp \
    loglistview.cpp \
    logsearchnoresultwidget.cpp

RESOURCES +=         resources.qrc

HEADERS += \
    logcollectormain.h \
    displaycontent.h \
    logfileparser.h \
    filtercontent.h \
    structdef.h \
    logtreeview.h \
    journalwork.h \
    logpasswordauth.h \
    logexportwidget.h \
    utils.h \
    loglistview.h \
    logsearchnoresultwidget.h


TRANSLATIONS += translations/dde-log-viewer.ts

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/dde-log-viewer

target.path = $$INSTROOT$$BINDIR
desktop.path = $$INSTROOT$$APPDIR
desktop.files = $$PWD/dde-log-viewer.desktop

translations.path = /usr/share/dde-log-viewer/translations
translations.files = $$PWD/translations/*.qm

icon_files.path = /usr/share/icons/hicolor/scalable/apps
icon_files.files = $$PWD/images/log.svg


DISTFILES += \
    translations/dde-log-viewer.ts

INSTALLS += target desktop translations icon_files
