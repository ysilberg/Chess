cmake_minimum_required(VERSION 3.16)
project(ChessProjectMagshimim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(chess_core
    Project1/Board.cpp Project1/Piece.cpp Project1/Pwn.cpp Project1/Rook.cpp
    Project1/Knight.cpp Project1/Bishop.cpp Project1/Queen.cpp Project1/King.cpp)
target_include_directories(chess_core PUBLIC Project1)
if(MSVC)
    target_compile_options(chess_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(chess_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_executable(chess_tests tests/test_chess.cpp)
target_link_libraries(chess_tests PRIVATE chess_core)
add_test(NAME chess_core_tests COMMAND chess_tests)

if(WIN32)
    add_executable(chess_app Project1/Source.cpp Project1/Manager.cpp Project1/stdafx.cpp)
    target_link_libraries(chess_app PRIVATE chess_core)
    target_include_directories(chess_app PRIVATE Project1)
endif()
