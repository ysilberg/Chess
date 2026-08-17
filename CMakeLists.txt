cmake_minimum_required(VERSION 3.16)
project(ChessProjectMagshimim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(chess_core
    src/Board.cpp src/Piece.cpp src/Pawn.cpp src/Rook.cpp
    src/Knight.cpp src/Bishop.cpp src/Queen.cpp src/King.cpp)
target_include_directories(chess_core PUBLIC include)
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
    add_executable(chess_app app/Source.cpp app/Manager.cpp app/stdafx.cpp)
    target_link_libraries(chess_app PRIVATE chess_core)
    target_include_directories(chess_app PRIVATE app)
endif()
