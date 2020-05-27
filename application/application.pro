QT += core gui dtkwidget dtkgui gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-log-viewer
TEMPLATE = app
CONFIG += c++11 link_pkgconfig

LIBS += -lsystemd

DEFINES += USE_POLKIT ENABLE_INACTIVE_DISPLAY

LIBS += -lsystemd -licui18n -licuuc

include(../3rdparty/QtXlsxWriter/src/xlsx/qtxlsx.pri)
include(../3rdparty/docx/docx.pri)

SOURCES += \
        main.cpp \
    filtercontent.cpp \
    displaycontent.cpp \
    logcollectormain.cpp \
    logfileparser.cpp \
    logtreeview.cpp \
    journalwork.cpp \
    logexportwidget.cpp \
    utils.cpp \
    loglistview.cpp \
    logperiodbutton.cpp \
    logviewheaderview.cpp \
    logviewitemdelegate.cpp \
    logiconbutton.cpp \
    logspinnerwidget.cpp \
    logdetailinfowidget.cpp \
    logauththread.cpp \
    logapplicationhelper.cpp \
    logapplicationparsethread.cpp \
    model/log_sort_filter_proxy_model.cpp \
    common/collator.cpp

RESOURCES +=         resources.qrc

HEADERS += \
    logcollectormain.h \
    displaycontent.h \
    logfileparser.h \
    filtercontent.h \
    structdef.h \
    logtreeview.h \
    journalwork.h \
    logexportwidget.h \
    utils.h \
    loglistview.h \
    logperiodbutton.h \
    logviewheaderview.h \
    logviewitemdelegate.h \
    logiconbutton.h \
    logspinnerwidget.h \
    logdetailinfowidget.h \
    logauththread.h \
    logapplicationhelper.h \
    logapplicationparsethread.h \
    wtmpparse.h \
    model/log_sort_filter_proxy_model.h \
    common/collator.h

!system(deepin-policy-ts-convert policy2ts com.deepin.pkexec.logViewerAuth.policy.tmp ../translations/policy): message("Failed policy to ts")
!system(deepin-policy-ts-convert ts2policy com.deepin.pkexec.logViewerAuth.policy.tmp policy-install-translation com.deepin.pkexec.logViewerAuth.policy) {
    system(cp com.deepin.pkexec.logViewerAuth.policy.tmp com.deepin.pkexec.logViewerAuth.policy)
}

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-log-viewer

target.path = $$INSTROOT$$BINDIR
desktop.path = $$INSTROOT$$APPDIR
desktop.files = $$PWD/deepin-log-viewer.desktop

# Automating generation .qm files from .ts files
#!system($$PWD/translate_generation.sh): error("Failed to generate translation")

CONFIG(release, debug|release) {
    TRANSLATIONS = $$files( ../translations/*.ts)
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    for(tsfile, TRANSLATIONS) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
#    #将qm文件添加到安装包
#    dtk_translations.path = /usr/share/$$TARGET/translations
#    dtk_translations.files = $$PWD/translations/*.qm
#    INSTALLS += dtk_translations
}

translations.path = /usr/share/deepin-log-viewer/translations
translations.files = ../translations/*.qm

icon_files.path = /usr/share/icons/hicolor/scalable/apps
icon_files.files = $$PWD/assets/images/deepin-log-viewer.svg

policy.path = /usr/share/polkit-1/actions
policy.files = $$PWD/com.deepin.pkexec.logViewerAuth.policy


INSTALLS += target desktop translations icon_files policy

DISTFILES += \
    ../translations/deepin-log-viewer_ar.ts \
    ../translations/deepin-log-viewer_ca.ts \
    ../translations/deepin-log-viewer_cs.ts \
    ../translations/deepin-log-viewer_da.ts \
    ../translations/deepin-log-viewer_de.ts \
    ../translations/deepin-log-viewer_es.ts \
    ../translations/deepin-log-viewer_fi.ts \
    ../translations/deepin-log-viewer_fr.ts \
    ../translations/deepin-log-viewer_it.ts \
    ../translations/deepin-log-viewer_ko.ts \
    ../translations/deepin-log-viewer_ms.ts \
    ../translations/deepin-log-viewer_nl.ts \
    ../translations/deepin-log-viewer_pl.ts \
    ../translations/deepin-log-viewer_pt.ts \
    ../translations/deepin-log-viewer_pt_BR.ts \
    ../translations/deepin-log-viewer_ru.ts \
    ../translations/deepin-log-viewer_sr.ts \
    ../translations/deepin-log-viewer_tr.ts \
    ../translations/deepin-log-viewer_uk.ts \
    ../translations/deepin-log-viewer_zh_CN.ts \
    ../translations/deepin-log-viewer_zh_TW.ts
