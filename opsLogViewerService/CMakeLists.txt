cmake_minimum_required(VERSION 3.10)

project(ops-log-viewer-service)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.service"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.conf"
    )

find_package(Qt5 COMPONENTS Core DBus REQUIRED)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::DBus
    )

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES org.deepin.logviewer.ops.conf DESTINATION share/dbus-1/system.d/)
install(FILES org.deepin.logviewer.ops.service DESTINATION share/dbus-1/system-services/)
install(FILES deepin-log-viewer-ops.service DESTINATION lib/systemd/system)
