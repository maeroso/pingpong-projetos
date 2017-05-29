cmake_minimum_required(VERSION 3.7)
project(p08)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        datatypes.h
        pingpong-join.c
        pingpong.c
        pingpong.h
        queue.c
        queue.h)

add_executable(p08 ${SOURCE_FILES})