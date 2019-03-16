cmake_minimum_required (VERSION 2.8)

add_compile_options(-std=c++11)

# boost 目录
INCLUDE_DIRECTORIES(/usr/local/include)
LINK_DIRECTORIES(/usr/local/lib)

project (HttpServer)

aux_source_directory(src DIR_SRCS)
ADD_SUBDIRECTORY(src/utils)

add_library(httpserver ${DIR_SRCS})
target_link_libraries(httpserver httpserver-utils boost_system)

add_executable(run_server example/main.cpp)
target_link_libraries(run_server httpserver)
