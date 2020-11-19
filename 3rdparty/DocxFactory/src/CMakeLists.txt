# After compilation it will destribute a static lib into a lib folder
# and destributes all header files into an include folder.

cmake_minimum_required(VERSION 3.13)
if (NOT DEFINED VERSION)
    set(VERSION 1.2.2)
endif ()
set (EXE_NAME docxFactoryTest)
project(${EXE_NAME} )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")#${EXE_NAME}
#-ldtkwidget -ldtkgui -ldtkcore
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif ()

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(UNICODE false CACHE BOOL "Use Unicode macro")





file(GLOB SRCS
    "src/*.cpp"
    )

add_executable(${EXE_NAME} ${SRCS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
