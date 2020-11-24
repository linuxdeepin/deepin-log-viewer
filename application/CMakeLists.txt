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
    Qt5::Concurrent
#   ${DtkCore_LIBRARIES}
#   ${DtkGUI_LIBRARIES}
#    ${DtkWidget_LIBRARIES}

#dtkwidget
#dtkcore
#dtkgui
#${DtkWidget_LDFLAGS}
#${DtkGui_LDFLAGS}
#${DtkCore_LDFLAGS}
#    ${ICUIL18N_LIBRARIES}
#    ${ICUUC_LIBRARIES}
#     ${SYSTEMD_LIBRARIES}
)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ldtkwidget -ldtkgui -ldtkcore")#${EXE_NAME}
#-ldtkwidget -ldtkgui -ldtkcore
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif ()

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    # generate qm files
    execute_process(COMMAND bash "assets/translate_generation.sh"
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE trans_var OUTPUT_VARIABLE trans_out ERROR_VARIABLE trans_error)
                message("trans_var ${trans_var} ")#trans_out ${trans_out} trans_error ${trans_error}
    execute_process(COMMAND deepin-policy-ts-convert "policy2ts" "assets/com.deepin.pkexec.logViewerAuth.policy.tmp" "../translations/policy"
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE policy2ts_var OUTPUT_VARIABLE policy2ts_out ERROR_VARIABLE policy2ts_error)
                 message("policy2ts_var ${policy2ts_var} policy2ts_out ${policy2ts_out} policy2ts_error ${policy2ts_error}")
    execute_process(COMMAND deepin-policy-ts-convert "ts2policy" "assets/com.deepin.pkexec.logViewerAuth.policy.tmp" "policy-install-translation" "assets/com.deepin.pkexec.logViewerAuth.policy"
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE ts2policy_var OUTPUT_VARIABLE ts2policy_out ERROR_VARIABLE ts2policy_error)
                 message("ts2policy_var ${ts2policy_var} ts2policy_out ${ts2policy_out} ts2policy_error ${ts2policy_error}")
    execute_process(COMMAND cp "assets/com.deepin.pkexec.logViewerAuth.policy.tmp" "assets/com.deepin.pkexec.logViewerAuth.policy"
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE cp_var OUTPUT_VARIABLE cp_out ERROR_VARIABLE cp_error)
                   message("cp_var ${cp_var} cp_out ${cp_out} cp_error ${cp_error}")


execute_process(COMMAND deepin-policy-ts-convert "policy2ts" "assets/com.deepin.pkexec.logViewerTruncate.policy.tmp" "../translations/policy"
                                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE policy2ts_var OUTPUT_VARIABLE policy2ts_out ERROR_VARIABLE policy2ts_error)
                                message("policy2ts_var ${policy2ts_var} policy2ts_out ${policy2ts_out} policy2ts_error ${policy2ts_error}")
 execute_process(COMMAND deepin-policy-ts-convert "ts2policy" "assets/com.deepin.pkexec.logViewerTruncate.policy.tmp" "policy-install-translation" "assets/com.deepin.pkexec.logViewerTruncate.policy"
                                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE ts2policy_var OUTPUT_VARIABLE ts2policy_out ERROR_VARIABLE ts2policy_error)
                                message("ts2policy_var ${ts2policy_var} ts2policy_out ${ts2policy_out} ts2policy_error ${ts2policy_error}")
execute_process(COMMAND cp "assets/com.deepin.pkexec.logViewerTruncate.policy.tmp" "assets/com.deepin.pkexec.logViewerTruncate.policy"
                                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE cp_var OUTPUT_VARIABLE cp_out ERROR_VARIABLE cp_error)
  message("cp_var ${cp_var} cp_out ${cp_out} cp_error ${cp_error}")
endif ()

add_definitions(-DUSE_POLKIT -DENABLE_INACTIVE_DISPLAY)
# Find the library
find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkGui REQUIRED)
find_package(DtkCore REQUIRED)
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
     logexportthread.cpp
     model/log_sort_filter_proxy_model.cpp
     common/collator.cpp
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

    file(GLOB ALL_SUB RELATIVE ${root_dir} ${root_dir}/*) # 获得当前目录下的所有文件，让如ALL_SUB列表中
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

add_executable (${EXE_NAME} ${APP_CPP_FILES} ${APP_QRC_FILES} ${DOCX_CPP_FILES} ${XLSX_CPP_FILES} ${TEST_ACCESSIBLE_HEADERS_FILES})
include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/docx")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/QtXlsxWriter/src/xlsx")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${DtkWidget_INCLUDE_DIRS})
include_directories(${DtkCore_INCLUDE_DIRS})
include_directories(${DtkGui_INCLUDE_DIRS})
include_directories( ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
message("dtk lib------- DtkWidget_LIBRARIES: ${DtkWidget_LIBRARIES}")#"---- DtkCore_LIBRARIES:${DtkCore_LIBRARIES} ---DtkGUI_LIBRARIES: ${DtkGUI_LIBRARIES}")
target_link_libraries(${EXE_NAME}  ${LINK_LIBS} -lsystemd -licui18n -licuuc )
target_link_libraries(${EXE_NAME} ${DtkCore_LIBRARIES})
target_link_libraries(${EXE_NAME} ${DtkGUI_LIBRARIES})
target_link_libraries(${EXE_NAME} ${DtkWidget_LIBRARIES})

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

                # Install files
                install(TARGETS ${EXE_NAME} DESTINATION bin)





