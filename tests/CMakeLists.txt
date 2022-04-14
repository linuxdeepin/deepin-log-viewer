cmake_minimum_required(VERSION 3.13)

if (NOT DEFINED VERSION)
    set(VERSION 1.2.2)
endif ()
configure_file(../application/environments.h.in environments.h @ONLY)
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
#安全测试加固编译参数
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -z relro -z now -z noexecstack -pie")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -z relro -z now -z noexecstack -pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fstack-protector-all")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -fstack-protector-all")
#-ldtkwidget -ldtkgui -ldtkcore
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif ()

set(CMAKE_SAFETYTEST "${CMAKE_SAFETYTEST_ARG}")
if(CMAKE_SAFETYTEST STREQUAL "")
    set(CMAKE_SAFETYTEST "CMAKE_SAFETYTEST_ARG_OFF")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${CMAKE_SAFETYTEST}")
if(CMAKE_SAFETYTEST STREQUAL "CMAKE_SAFETYTEST_ARG_ON")
  #安全测试选项
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=undefined,address -O2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=undefined,address -O2")
endif()

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
find_package(XercesC)
find_package(ZLIB)

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
     ../application/logsettings.cpp
     ../application/wtmpparse.cpp
     ../application/DebugTimeManager.cpp
     ../application/logdetailedit.cpp
     ../application/dbusproxy/dldbusinterface.cpp
     ../application/dbusproxy/dldbushandler.cpp
     ../application/viewsortfilter.cpp
     ../application/logallexportthread.cpp
)
FILE(GLOB qrcFiles
    ../application/assets/resources.qrc
)

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

find_library(ICUIL18N_LIB icui18n)
find_library(ICUUC_LIB icuuc)

#--------------------libxlsx源码引入---------------------------------
file(GLOB LXW_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/libxlsxwriter/src/*.c)
file(GLOB_RECURSE LXW_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/libxlsxwriter/include *.h)

set (MINIZIP_SOURCES
    ../3rdparty/minizip/ioapi.c
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
include_directories(${XercesC_INCLUDE_DIRS})
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
find_package(PolkitQt5-1)
FILE(GLOB allTestSource
src/*.cpp
common/*.cpp
dbusproxy/*.cpp
)
include_directories(src)


add_executable (${PROJECT_NAME_TEST} ${allSource} ${APP_QRC_FILES} ${APP_QM_FILES} ${qrcFiles}   ${allTestSource} ${MINIZIP_SOURCES} ${LXW_SOURCES} ${DOCXFAC_SOURCES} ${DOCXFAC_SOURCES_C} ${TMPFILE_SOURCES} ${MD5SOURCES})



target_link_libraries(${PROJECT_NAME_TEST}  ${LINK_LIBS} ${DFrameworkDBus_LIBRARIES} -lsystemd -licui18n -licuuc ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread -ldl -fPIC)
#当binutils>=2.22时，对于链接的静态库链接的静态库，cmake并不会递归链接，所以需要手动链接libxlsxwriter需要的lzlib
target_link_libraries(${PROJECT_NAME_TEST} ${ZLIB_LIBRARIES})
target_link_libraries(${PROJECT_NAME_TEST} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME_TEST} ${RapidJSON_LIBRARIES})
#target_link_libraries(${PROJECT_NAME_TEST} ${ImageMagick_LIBRARIES})
target_link_libraries(${PROJECT_NAME_TEST} ${XercesC_LIBRARIES})
target_link_libraries(${PROJECT_NAME_TEST} PolkitQt5-1::Agent)
