cmake_minimum_required (VERSION 2.8)

#set(CMAKE_SKIP_BUILD_RPATH TRUE)
add_compile_options(-std=c++11)

LINK_DIRECTORIES(/usr/local/lib64)

project (HttpServer)

aux_source_directory(src DIR_SRCS)

add_library(httpserver SHARED ${DIR_SRCS})
target_link_libraries(httpserver boost_system boost_iostreams ssl crypto pthread)

add_executable(run_server example/server_test.cpp)
target_link_libraries(run_server httpserver)

add_executable(client_test example/client_test.cpp)
target_link_libraries(client_test httpserver)
