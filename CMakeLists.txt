cmake_minimum_required(VERSION 3.6)
project(p00)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        queue.h
        testafila.c queue.c)

add_executable(p00 ${SOURCE_FILES})