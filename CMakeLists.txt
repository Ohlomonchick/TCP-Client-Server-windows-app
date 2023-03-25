cmake_minimum_required(VERSION 3.24)
project(client)
project(sockets)

set(CMAKE_CXX_STANDARD 20)

add_executable(sockets server.cpp app_entity.h)
target_link_libraries(sockets ws2_32)

add_executable(client client.cpp app_entity.h)
target_link_libraries(client ws2_32)