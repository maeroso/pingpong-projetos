cmake_minimum_required(VERSION 3.7)
project(p05)

set(CMAKE_C_STANDARD 11)
add_definitions(-D_XOPEN_SOURCE=700)
set(SOURCE_FILES
        queue.c
        pingpong.c
        pingpong-preempcao.c
#        pingpong-preempcao.txt
#        signal.c
#        timer.c
        )

add_executable(p05 ${SOURCE_FILES})