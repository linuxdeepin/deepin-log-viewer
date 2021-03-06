cmake_minimum_required(VERSION 3.7)

if (NOT DEFINED VERSION)
    set(VERSION 1.2.2)
endif ()
#这里项目名称绝对不能和编译出的target名称一样
project(logViewerService)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(APP_RES_DIR "assets")
set(APP_SERVICE "${APP_RES_DIR}/data/com.deepin.logviewer.service")
set(APP_CONFIG "${APP_RES_DIR}/data/com.deepin.logviewer.conf")

# Find the library
find_package(PkgConfig REQUIRED)
find_package(DtkGui REQUIRED)
find_package(Qt5 COMPONENTS
    Core
    Widgets
    DBus
REQUIRED)

set(LINK_LIBS
    Qt5::Core
    Qt5::DBus
    ${DtkCore_LIBRARIES}
)

file(GLOB ALL_SOURCES "*.cpp")
file(GLOB ALL_HEADERS "*.h")

include_directories(../application)
add_executable(${PROJECT_NAME} ${ALL_SOURCES} ${ALL_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${DtkWidget_INCLUDE_DIRS} ${OBJECT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME}
    ${LINK_LIBS}
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)
set(CMAKE_INSTALL_PREFIX /usr)

# Install files
install(TARGETS ${PROJECT_NAME} DESTINATION lib/deepin-daemon/)
install(FILES ${APP_SERVICE} DESTINATION share/dbus-1/system-services/)
install(FILES ${APP_CONFIG} DESTINATION share/dbus-1/system.d/)
