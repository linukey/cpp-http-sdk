cmake_minimum_required (VERSION 2.8)

#set(CMAKE_SKIP_BUILD_RPATH TRUE)
add_compile_options(-std=c++11 -g)

include_directories(/usr/include/x86_64-linux-gnu/qt5
                    /usr/include/x86_64-linux-gnu/qt5/QtWidgets
                    /usr/include/x86_64-linux-gnu/qt5/QtCore
                    /usr/include/x86_64-linux-gnu/qt5/QtWebKitWidgets
                    /usr/include/x86_64-linux-gnu/qt5/QtWebKit
                    /usr/include/x86_64-linux-gnu/qt5/QtNetwork)
LINK_DIRECTORIES(/usr/local/lib64)

project (HttpServer)

aux_source_directory(src DIR_SRCS)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Network REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)

add_library(httpserver SHARED ${DIR_SRCS})
target_link_libraries(httpserver boost_system boost_iostreams ssl crypto pthread Qt5Widgets Qt5WebKitWidgets Qt5WebKit Qt5Core Qt5Gui Qt5Network)

add_executable(run_server example/server_test.cpp)
target_link_libraries(run_server httpserver Qt5Widgets Qt5WebKitWidgets Qt5WebKit Qt5Core Qt5Gui Qt5Network)

add_executable(client_test example/client_test.cpp)
target_link_libraries(client_test httpserver Qt5Widgets Qt5WebKitWidgets Qt5WebKit Qt5Core Qt5Gui Qt5Network)
