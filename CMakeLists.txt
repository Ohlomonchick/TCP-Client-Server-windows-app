cmake_minimum_required(VERSION 3.24)
project(sockets)

set(CMAKE_CXX_STANDARD 20)

add_executable(sockets server.cpp client.cpp socket_entity.cpp socket_entity.h)
