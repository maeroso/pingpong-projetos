cmake_minimum_required(VERSION 3.6)
project(p01)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        contexts.c)

add_executable(p01 ${SOURCE_FILES})