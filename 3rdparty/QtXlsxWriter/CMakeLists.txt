cmake_minimum_required(VERSION 3.7)

if (NOT DEFINED VERSION)
    set(VERSION 1.2.2)
endif ()

project(application)
set (EXE_NAME deepin-log-viewer)
set(LINK_LIBS
    Qt5::Core
    Qt5::Widgets
)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")#${EXE_NAME}

#add_executable (${EXE_NAME} ${APP_CPP_FILES} ${APP_HEADERS_FILES} ${DC_QRC_FILES})
#add_executable (${EXE_NAME} main.cpp filtercontent.cpp)

set (APP_CPP_FILES
    main.cpp
filtercontent.cpp
displaycontent.cpp
logcollectormain.cpp
logfileparser.cpp
logtreeview.cpp
journalwork.cpp
logexportwidget.cpp
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
model/log_sort_filter_proxy_model.cpp
common/collator.cpp
    )
set (APP_QRC_FILES
resources.qrc
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
add_executable (${EXE_NAME} ${APP_CPP_FILES} ${DC_QRC_FILES})
# Find the library
find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkCore REQUIRED)
target_include_directories(${APP_BIN_NAME}
    PUBLIC ${DtkWidget_INCLUDE_DIRS} ${OBJECT_BINARY_DIR})

#set(CMAKE_INSTALL_PREFIX /usr)
#set(BINDIR binr)
#isEmpty(BINDIR):BINDIR=/usr/bin
#isEmpty(APPDIR):APPDIR=/usr/share/applications
#isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-log-viewer
# Install files
install(TARGETS ${EXE_NAME} DESTINATION bin)
set(CMAKE_INSTALL_PREFIX /usr)

file(GLOB APP_QM_FILES "../translations/*.qm")
install(FILES ${APP_QM_FILES} DESTINATION share/deepin-log-viewer/translations)
install(FILES deepin-log-viewer.desktop DESTINATION share/applications)
install(FILES assets/images/deepin-log-viewer.svg
            DESTINATION share/icons/hicolor/scalable/apps/)
install(FILES com.deepin.pkexec.logViewerAuth.policy
                    DESTINATION share/polkit-1/actions)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${EXE_NAME} ${LINK_LIBS} )
