#定义需要的cmake版本
cmake_minimum_required(VERSION 3.10)

# 设置工程名字
project(log-viewer-Plugin VERSION 0.1.0)
set(TARGET_NAME logviewerplugin)
set(CMD_NAME logviewerplugin)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#
set(CMAKE_AUTOMOC ON)

set(COMPILE_OPTIONS -std=c++11)

#预定义宏
add_compile_definitions (LOGVIEWERPLUGIN_LIBRARY)

#Qt需要的包
#set(QtModule Core Gui Widgets Svg Xml DBus Concurrent PrintSupport LinguistTools)
set(LINK_LIBS
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::Xml
    Qt5::DBus
    Qt5::Concurrent
)

#先查找到这些qt相关的模块以供链接使用
#find_package(Qt5 REQUIRED ${QtModule})
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
find_package(RapidJSON)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
find_package(Boost)
find_package(XercesC)
find_package(ZLIB)
find_package(PolkitQt5-1)

list(APPEND LXW_PRIVATE_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS})

#设置输出目录
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../deepin-album/lib)

#包含目录
include_directories(${CMAKE_INCLUDE_CURRENT_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DLITE_DIV )
add_definitions( -DUSE_UNIONIMAGE )
add_definitions( -DCMAKE_BUILD )

#库目录
aux_source_directory(logviewerplugin allSources)

find_package(PkgConfig REQUIRED)
pkg_check_modules(3rd_lib REQUIRED
    dtkwidget
    dtkcore
    dtkgui
    )

# Find the library
find_package(DFrameworkdbus REQUIRED)
find_package(RapidJSON)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
find_package(Boost)
find_package(XercesC)
find_package(ZLIB)
find_package(PolkitQt5-1)

#需要打开的头文件
FILE (GLOB allHeaders "*.h" "*/*.h" "*/*/*.h" "*/*/*/*.h")
#需要打开的代码文件
FILE (GLOB allSources
    "*.cpp"                                   "*.c"
    "*/*.cpp"                                 "*/*.c"
    "*/*/*.cpp"                              "*/*/*.c"
    "*/*/*/*.cpp"                           "*/*/*/*.c"
    )
file(GLOB_RECURSE RESOURCES "*.qrc")

#------------------添加第三方库begins--------------------------#
#使用第三方库需要用到的一个包

set(INC_DIR /usr/include/)
set(LINK_DIR /usr/lib/)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})

set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

file(GLOB TS LIST_DIRECTORIES false translations/liblogviewerplugin*.ts)

set_source_files_properties(${TS} PROPERTIES OUTPUT_LOCATION ${PROJECT_SOURCE_DIR}/translations)

qt5_create_translation(QM ${allSources}  ${GLOB})

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    qt5_create_translation(QM ${allSources} ${TS})
endif ()

# 设置不删除生成的文件夹内容文件
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

#编译为库
add_library(logviewerplugin SHARED ${allHeaders} ${allSources} ${RESOURCES} ${QM})
#使用库
#qt5_use_modules(logviewerplugin ${QtModule})

#将库安装到指定位置
set(PREFIX /usr)
set(${TARGET_NAME} ${CMAKE_INSTALL_LIBDIR})

set_target_properties(${TARGET_NAME} PROPERTIES VERSION 0.1.0 SOVERSION 0.1)


#target_include_directories(${CMD_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS}  )
#target_link_libraries(logviewerplugin ${3rd_lib_LIBRARIES})
target_compile_options(logviewerplugin PRIVATE ${COMPILE_OPTIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${DtkWidget_INCLUDE_DIRS})
include_directories(${DtkCore_INCLUDE_DIRS})
include_directories(${DtkGui_INCLUDE_DIRS})
include_directories( ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
include_directories( ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})
include_directories( ${DFrameworkDBus_INCLUDE_DIRS})
include_directories( ${Boost_INCLUDE_DIRS})
include_directories( ${ZLIB_INCLUDE_DIRS})
message("ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS}")
include_directories( ${RapidJSON_INCLUDE_DIRS})
#include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${XercesC_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}  ${LINK_LIBS} ${DFrameworkDBus_LIBRARIES}  -lsystemd -licui18n -licuuc  -ldl -fPIC)
target_link_libraries(${TARGET_NAME} ${DtkWidget_LIBRARIES})
target_link_libraries(${TARGET_NAME} ${DtkCore_LIBRARIES})
target_link_libraries(${TARGET_NAME} ${DtkGUI_LIBRARIES})
#当binutils>=2.22时，对于链接的静态库链接的静态库，cmake并不会递归链接，所以需要手动链接libxlsxwriter需要的lzlib
target_link_libraries(${TARGET_NAME} ${ZLIB_LIBRARIES})
target_link_libraries(${TARGET_NAME} ${Boost_LIBRARIES})
target_link_libraries(${TARGET_NAME} ${RapidJSON_LIBRARIES})
target_link_libraries(${TARGET_NAME} ${XercesC_LIBRARIES})
target_link_libraries(${TARGET_NAME} PolkitQt5-1::Agent)

include(GNUInstallDirs)
configure_file(liblogviewerplugin.pc.in ${PROJECT_BINARY_DIR}/liblogviewerplugin.pc @ONLY)

install(TARGETS ${CMD_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
    src/structdef.h
    DESTINATION include/liblogviewerplugin)
install(FILES
    src/plugininterfaces/logviewerplugininterface.h
    DESTINATION include/liblogviewerplugin/plugininterfaces)
install(FILES
    src/plugins/logviewerplugin.h
    DESTINATION include/liblogviewerplugin/plugins)

install(FILES ${PROJECT_BINARY_DIR}/liblogviewerplugin.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
#translations
install(DIRECTORY ${PROJECT_SOURCE_DIR}/translations
    DESTINATION ${PREFIX}/share/liblogviewerplugin
    FILES_MATCHING PATTERN "*.qm")

# 加速编译优化参数
if (NOT(CMAKE_BUILD_TYPE STREQUAL "Debug"))
    if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "mips64")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wl,-O1 -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi -z noexecstack")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wl,-O1 -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi -z noexecstack")
        SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
    else()
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wl,-O1 -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3  -Wl,-O1 -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections")
        SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
    endif()
endif()

if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "sw_64")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mieee")
    add_definitions( -DISSW_64 )
endif()
