QT += core gui dtkwidget dtkgui gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-log-viewer
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
#PKGCONFIG +=

LIBS += -L /usr/lib

DEFINES += USE_POLKIT


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
    logsearchnoresultwidget.cpp \
    logperiodbutton.cpp \
    logviewheaderview.cpp \
    logviewitemdelegate.cpp \
    logiconbutton.cpp \
    logspinnerwidget.cpp \
    logdetailinfowidget.cpp \
    logauththread.cpp

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
    logsearchnoresultwidget.h \
    logperiodbutton.h \
    logviewheaderview.h \
    logviewitemdelegate.h \
    logiconbutton.h \
    logspinnerwidget.h \
    logdetailinfowidget.h \
    logauththread.h


TRANSLATIONS += translations/deepin-log-viewer_zh_CN.ts


isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-log-viewer

target.path = $$INSTROOT$$BINDIR
desktop.path = $$INSTROOT$$APPDIR
desktop.files = $$PWD/deepin-log-viewer.desktop

# Automating generation .qm files from .ts files
!system($$PWD/translate_generation.sh): error("Failed to generate translation")

translations.path = /usr/share/deepin-log-viewer/translations
translations.files = $$PWD/translations/*.qm

icon_files.path = /usr/share/icons/hicolor/scalable/apps
icon_files.files = $$PWD/images/deepin-log-viewer.svg

policy.path = /usr/share/polkit-1/actions
policy.files = $$PWD/com.deepin.pkexec.logViewerAuth.policy


DISTFILES += \
    translations/deepin-log-viewer_zh_CN.ts

INSTALLS += target desktop translations icon_files policy
