cmake_minimum_required (VERSION 2.8)

add_compile_options(-std=c++11)

# boost 目录
INCLUDE_DIRECTORIES(/usr/local/include /usr/local/opt/openssl@1.1/include)
LINK_DIRECTORIES(/usr/local/lib /usr/local/opt/openssl@1.1/lib)

project (HttpServer)

aux_source_directory(src DIR_SRCS)

add_library(httpserver ${DIR_SRCS})
target_link_libraries(httpserver boost_system boost_iostreams ssl crypto pthread)

add_executable(run_server example/server_test.cpp)
target_link_libraries(run_server httpserver gumbo)

add_executable(client_test example/client_test.cpp)
target_link_libraries(client_test httpserver)
