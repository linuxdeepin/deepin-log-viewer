cmake_minimum_required(VERSION 3.7)

if (NOT DEFINED VERSION)
    set(VERSION 1.2.2)
endif ()

project(logViewerAuth LANGUAGES CXX)
set (EXE_NAME logViewerAuth)
set(LINK_LIBS
    Qt5::Core
)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=undefined,address -O2")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=undefined,address -O2")
set (AUTH_CPP_FILES
        main.cpp
       viewapplication.cpp
       viewapplication.h
    )
add_executable (${EXE_NAME}
    ${AUTH_CPP_FILES}
)

# Find the library
find_package(Qt5Core REQUIRED)
set(CMAKE_INSTALL_PREFIX /usr)
# Install files
install(TARGETS ${EXE_NAME} DESTINATION bin)
target_link_libraries(${EXE_NAME} ${LINK_LIBS} )
