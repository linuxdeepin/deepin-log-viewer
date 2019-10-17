QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dde_log_viewer
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget

TRANSLATIONS += translations/dde_log_viewer.ts
LIBS += -L /usr/lib

include(../thirdlib/QtXlsxWriter/src/xlsx/qtxlsx.pri)
include(../thirdlib/docx/docx.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


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
    loglistview.cpp

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
    loglistview.h


translations.path = /usr/share/dde_log_viewer/translations
translations.files = $$PWD/translations/*.qm

INSTALLS += translations

DISTFILES += \
    translations/dde_log_viewer_zh_CN.qm \
    translations/dde_log_viewer.ts
