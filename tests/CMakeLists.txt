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
configure_file(../application/environments.h.in environments.h @ONLY)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/test_output/)
set(LINK_LIBS
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::Xml
    Qt5::DBus
    Qt5::Concurrent
    Qt5::Test
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
#可访问private
ADD_COMPILE_OPTIONS(-fno-access-control)
ADD_COMPILE_OPTIONS(-fno-inline)
set (PROJECT_NAME_TEST deepin-log-viewer-test)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ldtkwidget -ldtkgui -ldtkcore")#${PROJECT_NAME_TEST}
#编译生成覆盖环境
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage")
#添加-fno-inline编译选项，禁止内联，能获取到函数地址，可以对内联函数进行打桩
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-inline")
#-ldtkwidget -ldtkgui -ldtkcore
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif ()


configure_file(../application/config.h.in config.h @ONLY)
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
find_package(Qt5Test REQUIRED)
find_package(DFrameworkdbus REQUIRED)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
add_definitions(${Qt5Test_DEFINITIONS})
find_package(RapidJSON)
find_package(Boost)
find_package(ImageMagick COMPONENTS Magick++)
find_package(ImageMagick COMPONENTS MagickWand)
find_package(ImageMagick COMPONENTS MagickCore)
find_package(XercesC)
find_package(ZLIB)
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


FILE(GLOB allSource
     ../application/filtercontent.cpp
     ../application/displaycontent.cpp
     ../application/logcollectormain.cpp
     ../application/logfileparser.cpp
     ../application/logtreeview.cpp
     ../application/journalwork.cpp
     ../application/logexportthread.cpp
     ../application/utils.cpp
     ../application/loglistview.cpp
     ../application/logperiodbutton.cpp
     ../application/logviewheaderview.cpp
     ../application/logviewitemdelegate.cpp
     ../application/logiconbutton.cpp
     ../application/logspinnerwidget.cpp
     ../application/logdetailinfowidget.cpp
     ../application/logauththread.cpp
     ../application/logapplicationhelper.cpp
     ../application/logapplicationparsethread.cpp
     ../application/journalbootwork.cpp
     ../application/exportprogressdlg.cpp
    ../application/logscrollbar.cpp
     ../application/model/log_sort_filter_proxy_model.cpp
     ../application/logcombox.cpp
     ../application/dbusmanager.cpp
     ../application/lognormalbutton.cpp
     ../application/logsearchedit.cpp
     ../application/logapplication.cpp
     ../application/sharedmemorymanager.cpp
     ../application/common/collator.cpp
       ../application/logsettings.cpp
      ../application/wtmpparse.cpp
      ../application/DebugTimeManager.cpp
      ../application/dbusproxy/dldbusinterface.cpp
      ../application/dbusproxy/dldbushandler.cpp
)
FILE(GLOB qrcFiles
    ../application/assets/resources.qrc
)




set (TEST_ACCESSIBLE_HEADERS_FILES
     ../application/accessible.h
     ../application/accessibledefine.h
    )
#include_directories(${DtkWidget_INCLUDE_DIRS})
#include_directories(${DtkGui_INCLUDE_DIRS})
#include_directories(${DtkCore_INCLUDE_DIRS})
#include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})
#include_directories(${Qt5Xml_LIBRARIES})
#include_directories(${Qt5Core_LIBRARIES})
#include_directories(${Qt5Widgets_LIBRARIES})

#add_definitions(-DQT_NO_DEBUG_OUTPUT)

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



#target_include_directories(${PROJECT_NAME_TEST}
#     ${DtkWidget_INCLUDE_DIRS} ${OBJECT_BINARY_DIR})

#file(GLOB_RECURSE application_source_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
#set(CMAKE_INSTALL_PREFIX /usr)
#set(BINDIR binr)
#isEmpty(BINDIR):BINDIR=/usr/bin
#isEmpty(APPDIR):APPDIR=/usr/share/applications
#isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-log-viewer

find_library(ICUIL18N_LIB icui18n)
find_library(ICUUC_LIB icuuc)

#--------------------libxlsx源码引入---------------------------------
file(GLOB LXW_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/libxlsxwriter/src/*.c)
file(GLOB_RECURSE LXW_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/libxlsxwriter/include *.h)

set (MINIZIP_SOURCES
    ../3rdparty/minizip/ioapi.c
#../3rdparty/minizip/miniunz.c
 ../3rdparty/minizip/minizip.c
../3rdparty/minizip/mztools.c
../3rdparty/minizip/unzip.c
../3rdparty/minizip/zip.c
    )

file(GLOB TMPFILE_SOURCES  ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/tmpfileplus/*.c)
 file(GLOB MD5SOURCES   ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/md5/*.c)
#---------------------------------------------
#---------------DOCXFACTORY源码引入----------------------
file(GLOB_RECURSE DOCXFAC_SOURCES  "${PROJECT_SOURCE_DIR}/3rdparty/DocxFactory/src/*.cpp" )
file(GLOB_RECURSE DOCXFAC_SOURCES_C  "${PROJECT_SOURCE_DIR}/3rdparty/DocxFactory/src/*.c" )
#-----------------------------------------------------

include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/libxlsxwriter")
include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/DocxFactory/include")
include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/md5")
include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/minizip")
include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/tmpfileplus")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_sub_directories_recursively("${CMAKE_CURRENT_SOURCE_DIR}/../application")
include_directories(${DtkWidget_INCLUDE_DIRS})
include_directories(${DtkCore_INCLUDE_DIRS})
include_directories(${DtkGui_INCLUDE_DIRS})
include_directories( ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
include_directories( ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})
include_directories( ${Qt5TestLib_LIBRARIES})
include_directories( ${DFrameworkDBus_INCLUDE_DIRS})
include_directories( ${Boost_INCLUDE_DIRS})
include_directories( ${ZLIB_INCLUDE_DIRS})
include_directories( ${RapidJSON_INCLUDE_DIRS})
include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${XercesC_INCLUDE_DIRS})
#include_directories(${Qt5Widgets_LIBRARIES}
#                                               ${Qt5Core_LIBRARIES}
#                                              ${Qt5DBus_LIBRARIES}
#                                              ${PROJECT_BINARY_DIR}
#                                              ${DtkWidget_INCLUDE_DIRS}
#                                              ${DtkCore_LIBRARIES}
#                                              ${DtkGUI_INCLUDE_DIRS}
#                                              ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
#                                              ${Qt5Xml_LIBRARIES})
#target_include_directories(${PROJECT_NAME_TEST} PUBLIC ${Qt5Widgets_LIBRARIES}
#                                               ${Qt5Core_LIBRARIES}
#                                              ${Qt5DBus_LIBRARIES}
#                                              ${PROJECT_BINARY_DIR}
#                                              ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
#                                              ${Qt5Xml_LIBRARIES})
#install(FILES ${ICUIL18N_LIB} ${ICUUC_LIB} ${SYSTEMD_LIB} DESTINATION lib)
#set(CMAKE_INSTALL_PREFIX /usr)

































find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})






#include_directories(googletest)
#include_directories(googletest/googlemock/include)
#include_directories(googletest/googletest/include)

##GTest & GMock
#add_subdirectory(googletest)



FILE(GLOB allTestSource
src/*.cpp
common/*.cpp
dbusproxy/*.cpp

# src/displaycontent_test.cpp
#  src/test_main.cpp
#  src/stuballthread.cpp
#src/exportprogressdlg_test.cpp
#src/dbusmanager_test.cpp
#src/filtercontent_test.cpp
#src/journalbootwork_test.cpp
#src/journalwork_test.cpp
#src/logapplication_test.cpp
#src/logapplicationhelper_test.cpp
#src/logapplicationparsethread_test.cpp
#src/logauththread_test.cpp
#src/logcollectormain_test.cpp
#src/logcombox_test.cpp
#src/logdetailinfowidget_test.cpp
#src/logexportthread_test.cpp
)
include_directories(src)


add_executable (${PROJECT_NAME_TEST} ${allSource} ${APP_QRC_FILES}   ${TEST_ACCESSIBLE_HEADERS_FILES} ${APP_QM_FILES} ${qrcFiles}   ${allTestSource} ${MINIZIP_SOURCES} ${LXW_SOURCES} ${DOCXFAC_SOURCES} ${DOCXFAC_SOURCES_C} ${TMPFILE_SOURCES} ${MD5SOURCES})



target_link_libraries(${PROJECT_NAME_TEST}  ${LINK_LIBS} ${DFrameworkDBus_LIBRARIES} -lsystemd -licui18n -licuuc ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread -ldl -fPIC)
#当binutils>=2.22时，对于链接的静态库链接的静态库，cmake并不会递归链接，所以需要手动链接libxlsxwriter需要的lzlib
target_link_libraries(${PROJECT_NAME_TEST} ${ZLIB_LIBRARIES})
target_link_libraries(${PROJECT_NAME_TEST} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME_TEST} ${RapidJSON_LIBRARIES})
target_link_libraries(${PROJECT_NAME_TEST} ${ImageMagick_LIBRARIES})
target_link_libraries(${PROJECT_NAME_TEST} ${XercesC_LIBRARIES})


#------------------------------ 创建'make test'指令---------------------------------------
add_custom_target(test
#    #执行mkdir -p 创建 coverageResult
#    COMMAND mkdir -p test/coverageResult
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


add_custom_command(TARGET test
    COMMAND echo " =================== CREAT LCOV REPROT BEGIN ==================== "

#    #1.清理原先的gcov信息
#    COMMAND lcov --directory ./test/CMakeFiles/${PROJECT_NAME_TEST}.dir --zerocounters
#    COMMAND lcov --directory ./test/CMakeFiles/${PROJECT_NAME}.dir --zerocounters
    COMMAND ${CMAKE_BINARY_DIR}/tests/${PROJECT_NAME_TEST}

#    #2.收集gcov信息到.info文件中
#    COMMAND lcov --directory . --capture --output-file ./test/coverageResult/${PROJECT_NAME}_Coverage.info

#    #3.过滤一些我们不感兴趣的文件的覆盖率信息
#    COMMAND echo " =================== do filter begin ==================== "
#    COMMAND lcov --remove ./test/coverageResult/${PROJECT_NAME}_Coverage.info
#    '*/${PROJECT_NAME_TEST}_autogen/*' '*/${PROJECT_NAME}_autogen/*' '*/usr/include/*' '*/dbuslogin1manager*' '*/test/*' '*/googletest/*' '*/3rdparty/*' '*/test/*' 'accessible.h' 'accessibledefine.h'
#    -o ./test/coverageResult/${PROJECT_NAME}_Coverage.info
#    COMMAND echo " =================== do filter end ==================== "

#    #3.将.info信息生成报告到reprot文件夹中
#    COMMAND genhtml -o ./test/coverageResult/report ./test/coverageResult/${PROJECT_NAME}_Coverage.info

#    COMMAND echo " =================== CREAT LCOV REPROT END ==================== "

#    COMMAND echo " Coverage files have been output to ${CMAKE_BINARY_DIR}/coverageResult/report "

    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# debian 会自动执行 make test
#'make test'命令依赖与我们的测试程序
add_dependencies(test ${PROJECT_NAME_TEST})























