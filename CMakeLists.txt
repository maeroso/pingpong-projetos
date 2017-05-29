cmake_minimum_required(VERSION 3.7)
project(p06)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
        datatypes.h
        pingpong-contab-prio.c
#        pingpong-contab.c
        pingpong.c
        pingpong.h
        queue.c
        queue.h)

add_executable(p06 ${SOURCE_FILES})