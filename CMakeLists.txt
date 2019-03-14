cmake_minimum_required (VERSION 2.8)

add_compile_options(-std=c++11)

# boost 目录
INCLUDE_DIRECTORIES(/usr/local/include)

project (HttpServer)

aux_source_directory(src DIR_SRCS)
ADD_SUBDIRECTORY(src/utils)

add_library(httpserver ${DIR_SRCS})
