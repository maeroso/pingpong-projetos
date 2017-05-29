cmake_minimum_required(VERSION 3.6)
project(p02)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

set(SOURCE_FILES
        queue.h queue.c
        datatypes.h
        pingpong-tasks1.c
        pingpong.h pingpong.c)

add_executable(p02 ${SOURCE_FILES})