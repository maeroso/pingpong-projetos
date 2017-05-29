cmake_minimum_required(VERSION 3.7)
project(p04)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES pingpong-scheduler.c pingpong.c queue.c)
add_executable(p04 ${SOURCE_FILES})