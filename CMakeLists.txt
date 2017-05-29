cmake_minimum_required(VERSION 3.7)
project(p07)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        datatypes.h
        pingpong-maintask.c
        pingpong.c
        pingpong.h
        queue.c
        queue.h)

add_executable(p07 ${SOURCE_FILES})