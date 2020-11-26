cmake_minimum_required(VERSION 3.13)

if (NOT DEFINED VERSION)
    set(VERSION 1.2.2)
endif ()
#set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
#set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
#project(application)
#add_subdirectory(../3rdparty/docx docx_binary_dir)
#add_subdirectory(../3rdparty/QtXlsxWriter)
configure_file(environments.h.in environments.h @ONLY)
set (EXE_NAME deepin-log-viewer)
set(LINK_LIBS
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::Xml
    Qt5::DBus
    Qt5::Concurrent
    ${DtkWidget_LIBRARIES}
    ${DtkCore_LIBRARIES}
    ${DtkGUI_LIBRARIES}

#${DtkWidget_LDFLAGS}
#${DtkGui_LDFLAGS}
#${DtkCore_LDFLAGS}
#    ${ICUIL18N_LIBRARIES}
#    ${ICUUC_LIBRARIES}
#     ${SYSTEMD_LIBRARIES}
)

configure_file(config.h.in config.h @ONLY)




set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ldtkwidget -ldtkgui -ldtkcore")#${EXE_NAME}
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--as-need -fPIE")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")
#-ldtkwidget -ldtkgui -ldtkcore
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif ()

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")



     execute_process(COMMAND deepin-desktop-ts-convert "init" "assets/deepin-log-viewer.desktop.tmp" "../translations/desktop"
                                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE desktop_transinit_var OUTPUT_VARIABLE desktop_transinit_out ERROR_VARIABLE desktop_transinit_error)
     execute_process(COMMAND deepin-desktop-ts-convert "desktop2ts" "assets/deepin-log-viewer.desktop.tmp" "../translations/desktop"
                                                                       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE desktop_trans_desktop2ts_var OUTPUT_VARIABLE desktop_trans_desktop2ts_out ERROR_VARIABLE desktop_trans_desktop2ts_error)
     execute_process(COMMAND deepin-desktop-ts-convert "ts2desktop" "assets/deepin-log-viewer.desktop.tmp" "../translations/desktop" "assets/deepin-log-viewer.desktop"
           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE desktop_trans_desktop2ts_var OUTPUT_VARIABLE desktop_trans_desktop2ts_out ERROR_VARIABLE desktop_trans_desktop2ts_error)

    # generate qm files
    execute_process(COMMAND bash "assets/translate_generation.sh"
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE trans_var OUTPUT_VARIABLE trans_out ERROR_VARIABLE trans_error)
                message("trans_var ${trans_var} ")#trans_out ${trans_out} trans_error ${trans_error}
 execute_process(COMMAND deepin-policy-ts-convert "init" "assets/com.deepin.pkexec.logViewerAuth.policy.tmp" "../translations/policy/auth"
                                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE policy2ts_init_var OUTPUT_VARIABLE policy2ts_init_out ERROR_VARIABLE policy2ts_init_error)

    execute_process(COMMAND deepin-policy-ts-convert "policy2ts" "assets/com.deepin.pkexec.logViewerAuth.policy.tmp" "../translations/policy/auth"
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE policy2ts_var OUTPUT_VARIABLE policy2ts_out ERROR_VARIABLE policy2ts_error)
                 message("policy2ts_var ${policy2ts_var} policy2ts_out ${policy2ts_out} policy2ts_error ${policy2ts_error}")
    execute_process(COMMAND deepin-policy-ts-convert "ts2policy" "assets/com.deepin.pkexec.logViewerAuth.policy.tmp" "../translations/policy/auth" "assets/com.deepin.pkexec.logViewerAuth.policy"
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE ts2policy_var OUTPUT_VARIABLE ts2policy_out ERROR_VARIABLE ts2policy_error)
                 message("ts2policy_var ${ts2policy_var} ts2policy_out ${ts2policy_out} ts2policy_error ${ts2policy_error}")
#    execute_process(COMMAND cp "assets/com.deepin.pkexec.logViewerAuth.policy.tmp" "assets/com.deepin.pkexec.logViewerAuth.policy"
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE cp_var OUTPUT_VARIABLE cp_out ERROR_VARIABLE cp_error)
#                   message("cp_var ${cp_var} cp_out ${cp_out} cp_error ${cp_error}")

 execute_process(COMMAND deepin-policy-ts-convert "init" "assets/com.deepin.pkexec.logViewerTruncate.policy.tmp" "../translations/policy/truncate"
                                                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE policy2ts_var OUTPUT_VARIABLE policy2ts_out ERROR_VARIABLE policy2ts_error)
execute_process(COMMAND deepin-policy-ts-convert "policy2ts" "assets/com.deepin.pkexec.logViewerTruncate.policy.tmp" "../translations/policy/truncate"
                                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE policy2ts_var OUTPUT_VARIABLE policy2ts_out ERROR_VARIABLE policy2ts_error)
                                message("policy2ts_var ${policy2ts_var} policy2ts_out ${policy2ts_out} policy2ts_error ${policy2ts_error}")
 execute_process(COMMAND deepin-policy-ts-convert "ts2policy" "assets/com.deepin.pkexec.logViewerTruncate.policy.tmp" "../translations/policy/truncate" "assets/com.deepin.pkexec.logViewerTruncate.policy"
                                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE ts2policy_var OUTPUT_VARIABLE ts2policy_out ERROR_VARIABLE ts2policy_error)
                                message("ts2policy_var ${ts2policy_var} ts2policy_out ${ts2policy_out} ts2policy_error ${ts2policy_error}")
#execute_process(COMMAND cp "assets/com.deepin.pkexec.logViewerTruncate.policy.tmp" "assets/com.deepin.pkexec.logViewerTruncate.policy"
#                                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE cp_var OUTPUT_VARIABLE cp_out ERROR_VARIABLE cp_error)
#  message("cp_var ${cp_var} cp_out ${cp_out} cp_error ${cp_error}")
endif ()

add_definitions(-DUSE_POLKIT -DENABLE_INACTIVE_DISPLAY)
# Find the library
find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkGui REQUIRED)
find_package(DtkCore REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(DFrameworkdbus REQUIRED)
find_package(Boost)
find_package(Zint)
find_package(RapidJSON)
#find_package(OpenOffice REQUIRED)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)

#pkg_search_module(DtkWidget REQUIRED dtkwidget)
#pkg_search_module(DtkGui REQUIRED dtkgui)
#pkg_search_module(DtkCore REQUIRED dtkcore)
#message(  "dtklib ${DtkWidget_LIBRARIES}
#    ${DtkCore_LIBRARIES}
#    ${DtkGUI_LIBRARIES}")


#message(  "dtkLDFLAGS ${DtkWidget_LDFLAGS}
#    ${DtkGui_LDFLAGS}
#    ${DtkCore_LDFLAGS} ")
#message(  "dtkinclude ${DtkWidget_INCLUDE_DIRS}
#    ${DtkGui_INCLUDE_DIRS}
#    ${DtkCore_INCLUDE_DIRS} ")
#find_package(Icuil18n)
#find_package(Icuuc)
#find_package(Systemd)
#if(ICUIL18N_FOUND)
#    set(CMAKE_AUTORCC ON)
#else(ICUIL18N_FOUND)
#message(ICUIL18N_FOUND "ICUIL18N not found!")
#endif(ICUIL18N_FOUND)


set (APP_CPP_FILES
    main.cpp
     filtercontent.cpp
     displaycontent.cpp
     logcollectormain.cpp
     logfileparser.cpp
     logtreeview.cpp
     journalwork.cpp
     logexportthread.cpp
     utils.cpp
     loglistview.cpp
     logperiodbutton.cpp
     logviewheaderview.cpp
     logviewitemdelegate.cpp
     logiconbutton.cpp
     logspinnerwidget.cpp
     logdetailinfowidget.cpp
     logauththread.cpp
     logapplicationhelper.cpp
     logapplicationparsethread.cpp
     journalbootwork.cpp
     exportprogressdlg.cpp
     logscrollbar.cpp
     model/log_sort_filter_proxy_model.cpp
     model/logbasemodel.cpp
     logcombox.cpp
     dbusmanager.cpp
     lognormalbutton.cpp
     logapplication.cpp
     sharedmemorymanager.cpp
    basesharedmemorymanager.cpp
     basesharedmemorymanager.h
     common/collator.cpp
     logsettings.cpp
     DebugTimeManager.cpp
     wtmpparse.cpp
     fileloader/logfileloader.cpp
     fileloader/logdatacontrol.cpp
     fileloader/xorgfileloader.cpp
    )
set (APP_QRC_FILES
assets/resources.qrc
)
set (DOCX_CPP_FILES
    ../3rdparty/docx/document.cpp
    ../3rdparty/docx/shared.cpp
    ../3rdparty/docx/text.cpp
    ../3rdparty/docx/parts/documentpart.cpp
    ../3rdparty/docx/opc/part.cpp
    ../3rdparty/docx/opc/packuri.cpp
    ../3rdparty/docx/opc/opcpackage.cpp
    ../3rdparty/docx/package.cpp
    ../3rdparty/docx/opc/parts/corepropertiespart.cpp
    ../3rdparty/docx/opc/coreproperties.cpp
    ../3rdparty/docx/opc/oxml.cpp
    ../3rdparty/docx/opc/constants.cpp
    ../3rdparty/docx/oxml/parts/oxmldocument.cpp
    ../3rdparty/docx/oxml/oxmltext.cpp
    ../3rdparty/docx/table.cpp
    ../3rdparty/docx/oxml/xmlchemy.cpp
    ../3rdparty/docx/oxml/oxmltable.cpp
    ../3rdparty/docx/oxml/oxmlshape.cpp
    ../3rdparty/docx/shape.cpp
    ../3rdparty/docx/parts/imagepart.cpp
    ../3rdparty/docx/image/image.cpp
    ../3rdparty/docx/opc/physpkgreader.cpp
    ../3rdparty/docx/opc/packagereader.cpp
    ../3rdparty/docx/opc/contenttypemap.cpp
    ../3rdparty/docx/opc/serializedrelationships.cpp
    ../3rdparty/docx/opc/rel.cpp
    ../3rdparty/docx/opc/packagewriter.cpp
    ../3rdparty/docx/opc/physpkgwriter.cpp
    ../3rdparty/docx/enums/enumtext.cpp
    ../3rdparty/docx/length.cpp
    )
set (XLSX_CPP_FILES
    ../3rdparty/QtXlsxWriter/src/xlsx/xlsxdocpropscore.cpp
    ../3rdparty/QtXlsxWriter/src/xlsx/xlsxdocpropsapp.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxrelationships.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxutility.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxsharedstrings.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxcontenttypes.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxtheme.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxformat.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxstyles.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxworkbook.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxabstractsheet.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxworksheet.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxchartsheet.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxzipwriter.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxdrawing.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxzipreader.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxdocument.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxcell.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxdatavalidation.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxcellreference.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxcellrange.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxrichstring.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxconditionalformatting.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxcolor.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxnumformatparser.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxdrawinganchor.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxmediafile.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxabstractooxmlfile.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxchart.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxsimpleooxmlfile.cpp
       ../3rdparty/QtXlsxWriter/src/xlsx/xlsxcellformula.cpp




    )
set (APP_HEADERS_FILES
   logcollectormain.h
  displaycontent.h
    logfileparser.h
    filtercontent.h
    structdef.h
    logtreeview.h
    journalwork.h
    logexportwidget.h
    utils.h
    loglistview.h
    logperiodbutton.h
    logviewheaderview.h
    logviewitemdelegate.h
    logiconbutton.h
    logspinnerwidget.h
    logdetailinfowidget.h
    logauththread.h
    logapplicationhelper.h
    logapplicationparsethread.h
    wtmpparse.h
    model/log_sort_filter_proxy_model.h
    common/collator.h
    )
file(GLOB DOCX_FACTORY_TEST
    ../3rdparty/DocxFactory/src/barcode/**
    ../3rdparty/DocxFactory/src/console/**
    ../3rdparty/DocxFactory/src/ConverterPrinter/**
    ../3rdparty/DocxFactory/src/DocxCompiler/**
    ../3rdparty/DocxFactory/src/DocxMerger/**
    ../3rdparty/DocxFactory/src/json/**
    ../3rdparty/DocxFactory/src/locale/**
    ../3rdparty/DocxFactory/src/math/**
    ../3rdparty/DocxFactory/src/opc/**
    ../3rdparty/DocxFactory/src/os/**
    ../3rdparty/DocxFactory/src/parser/**
    ../3rdparty/DocxFactory/src/str/**
    ../3rdparty/DocxFactory/src/time/**
    ../3rdparty/DocxFactory/src/unix/**
    ../3rdparty/DocxFactory/src/UnoUtil/**
    ../3rdparty/DocxFactory/src/util/**
    ../3rdparty/DocxFactory/src/win/**
    ../3rdparty/DocxFactory/src/WordProcessingCompiler/**
    ../3rdparty/DocxFactory/src/WordProcessingMerger/**
    ../3rdparty/DocxFactory/src/xml/**
    ../3rdparty/DocxFactory/src/zip/**
     ../3rdparty/DocxFactory/src/*.cpp
    )

set (TEST_ACCESSIBLE_HEADERS_FILES
     accessible.h
     accessibledefine.h
    )
#include_directories(${DtkWidget_INCLUDE_DIRS})
#include_directories(${DtkGui_INCLUDE_DIRS})
#include_directories(${DtkCore_INCLUDE_DIRS})
#include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})
#include_directories(${Qt5Xml_LIBRARIES})
#include_directories(${Qt5Core_LIBRARIES})
#include_directories(${Qt5Widgets_LIBRARIES})



# 5. 头文件
# 5.1. 定义函数，用于递归添加头文件
function(include_sub_directories_recursively root_dir)
    if (IS_DIRECTORY ${root_dir})               # 当前路径是一个目录吗，是的话就加入到包含目录
        #message("include dir: " ${root_dir})
        include_directories(${root_dir})
    endif()

    file(GLOB ALL_SUB RELATIVE ${root_dir} ${root_dir}/*) # 获得当前APP_HEADERS_FILES目录下的所有文件，让如ALL_SUB列表中
    foreach(sub ${ALL_SUB})
        if (IS_DIRECTORY ${root_dir}/${sub})
            include_sub_directories_recursively(${root_dir}/${sub}) # 对子目录递归调用，包含
        endif()
    endforeach()
endfunction()



#target_include_directories(${EXE_NAME}
#     ${DtkWidget_INCLUDE_DIRS} ${OBJECT_BINARY_DIR})

#file(GLOB_RECURSE application_source_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
#set(CMAKE_INSTALL_PREFIX /usr)
#set(BINDIR binr)
#isEmpty(BINDIR):BINDIR=/usr/bin
#isEmpty(APPDIR):APPDIR=/usr/share/applications
#isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-log-viewer

find_library(ICUIL18N_LIB icui18n)
find_library(ICUUC_LIB icuuc)
# ZLIB
find_package(ZLIB REQUIRED "1.0")
list(APPEND LXW_PRIVATE_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS})
message("zlib version: " ${ZLIB_VERSION})
#lupdate start
set(TS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../translations")
set(TS_FILES
    "${TS_DIR}/${EXE_NAME}.ts"
)
find_program(LUPDATE_EXECUTABLE lupdate)
foreach(_ts_file ${TS_FILES})
    execute_process(
        COMMAND ${LUPDATE_EXECUTABLE} -recursive ${CMAKE_SOURCE_DIR} -ts ${_ts_file})
endforeach()
#lupdate end

file (GLOB APP_TS_FILES translations/*.ts)
qt5_create_translation(APP_QM_FILES ${APP_TS_FILES} ${APP_QM_FILES})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
add_executable (${EXE_NAME} ${APP_CPP_FILES} ${APP_QRC_FILES} ${DOCX_CPP_FILES} ${XLSX_CPP_FILES} ${TEST_ACCESSIBLE_HEADERS_FILES} ${APP_QM_FILES} )
include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/docx")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/QtXlsxWriter/src/xlsx")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/common")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/ChardetDetector")
include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/libxlsxwriter")
include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/DocxFactory/include")
include_directories(${DtkWidget_INCLUDE_DIRS})
include_directories(${DtkCore_INCLUDE_DIRS})
include_directories(${DtkGui_INCLUDE_DIRS})
include_directories( ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
include_directories( ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})
include_directories( ${DFrameworkDBus_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Zint_INCLUDE_DIRS})
include_directories(${RapidJSON_INCLUDE_DIRS})

target_link_libraries(${EXE_NAME}  ${LINK_LIBS} -lsystemd -licui18n -licuuc log-ChardetDetector log-common   ${DFrameworkDBus_LIBRARIES}  ${CMAKE_BINARY_DIR}/3rdparty/libxlsxwriter/libxlsxwriter.a )
#这个excel库不用，静态链接后速度太慢
#include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/simplexlsx-code")
#target_link_libraries(${EXE_NAME} ${CMAKE_BINARY_DIR}/3rdparty/simplexlsx-code/libSimpleXlsx.a )

#当binutils>=2.22时，对于链接的静态库链接的静态库，cmake并不会递归链接，所以需要手动链接libxlsxwriter需要的lzlib
target_link_libraries(${EXE_NAME} ${ZLIB_LIBRARIES})
target_link_libraries(${EXE_NAME} ${Boost_LIBRARIES})
target_link_libraries(${EXE_NAME} ${Zint_LIBRARIES})
target_link_libraries(${EXE_NAME} ${RapidJSON_LIBRARIES})

#include_directories(${Qt5Widgets_LIBRARIES}
#                                               ${Qt5Core_LIBRARIES}
#                                              ${Qt5DBus_LIBRARIES}
#                                              ${PROJECT_BINARY_DIR}
#                                              ${DtkWidget_INCLUDE_DIRS}
#                                              ${DtkCore_LIBRARIES}
#                                              ${DtkGUI_INCLUDE_DIRS}
#                                              ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
#                                              ${Qt5Xml_LIBRARIES})
#target_include_directories(${EXE_NAME} PUBLIC ${Qt5Widgets_LIBRARIES}
#                                               ${Qt5Core_LIBRARIES}
#                                              ${Qt5DBus_LIBRARIES}
#                                              ${PROJECT_BINARY_DIR}
#                                              ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
#                                              ${Qt5Xml_LIBRARIES})
#install(FILES ${ICUIL18N_LIB} ${ICUUC_LIB} ${SYSTEMD_LIB} DESTINATION lib)
#set(CMAKE_INSTALL_PREFIX /usr)

file(GLOB APP_QM_FILES "../translations/*.qm")
install(FILES ${APP_QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/deepin-log-viewer/translations)
install(FILES assets/deepin-log-viewer.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES assets/com.deepin.pkexec.logViewerAuth.policy DESTINATION ${CMAKE_INSTALL_PREFIX}/share/polkit-1/actions)
install(FILES assets/images/deepin-log-viewer.svg
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps/)
install(FILES assets/com.deepin.pkexec.logViewerAuth.policy
                    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/polkit-1/actions)
install(FILES assets/com.deepin.pkexec.logViewerTruncate.policy DESTINATION ${CMAKE_INSTALL_PREFIX}/share/polkit-1/actions)
install(DIRECTORY 3rdparty/DocxFactory/lib DESTINATION lib/deepin-log-viewer/plugins/)
                # Install files
                install(TARGETS ${EXE_NAME} DESTINATION bin)





