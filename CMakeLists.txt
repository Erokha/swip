cmake_minimum_required(VERSION 3.21)
project(course_nets)

set(CMAKE_CXX_STANDARD 11)

add_executable(server server.cpp common.h common_includes.h)
add_executable(client client.cpp common.h common_includes.h)
add_executable(new_server newserver.cpp common.h common_includes.h ClientDescriptior/ClientDescriptor.h ClientDescriptior/ClientDescriptor.cpp)