cmake_minimum_required(VERSION 3.7)

if (NOT DEFINED VERSION)
    set(VERSION 1.2.2)
endif ()

project(logViewerAuth LANGUAGES CXX)
set (EXE_NAME logViewerAuth)
set(LINK_LIBS
    Qt${QT_DESIRED_VERSION}::Core
    PolkitQt${QT_DESIRED_VERSION}-1::Agent
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=undefined,address -O2")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=undefined,address -O2")

if(CMAKE_COVERAGE_ARG STREQUAL "CMAKE_COVERAGE_ARG_ON")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fprofile-arcs -ftest-coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fprofile-arcs -ftest-coverage")
endif()

#安全测试加固编译参数
string(APPEND CMAKE_EXE_LINKER_FLAGS " -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fstack-protector-all")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -fstack-protector-all")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
set (AUTH_CPP_FILES
        main.cpp
       viewapplication.cpp
       viewapplication.h
       ../application/qtcompat.h
    )
add_executable (${EXE_NAME}
    ${AUTH_CPP_FILES}
)

# Find the library
find_package(Qt${QT_DESIRED_VERSION} REQUIRED COMPONENTS Core)
find_package(PolkitQt${QT_DESIRED_VERSION}-1)
set(CMAKE_INSTALL_PREFIX /usr)
# Install files
install(TARGETS ${EXE_NAME} DESTINATION bin)
target_link_libraries(${EXE_NAME} ${LINK_LIBS} )
