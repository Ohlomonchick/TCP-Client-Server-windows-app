cmake_minimum_required(VERSION 3.24)
project(client)
project(server)

set(CMAKE_CXX_STANDARD 20)

add_executable(server server.cpp app_entity.h)
target_link_libraries(server ws2_32)

add_executable(client client.cpp app_entity.h)
target_link_libraries(client ws2_32)