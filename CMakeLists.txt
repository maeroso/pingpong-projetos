cmake_minimum_required(VERSION 3.6)
project(p03)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        pingpong-dispatcher.c pingpong.c queue.c)

add_executable(p03 ${SOURCE_FILES})

#[[
datatypes.h : definições de dados globais (esqueleto: datatypes.h)
    constantes e macros globais
    definições dos tipos e estruturas de dados globais
pingpong.h : interface do SO (fornecido: pingpong.h, não alterar)
    protótipos das funções públicas
pingpong.c : contém as definições internas do sistema:
    constantes e macros internas
    definições dos tipos e estruturas de dados internas
    funções internas
    implementações das funções públicas
]]
