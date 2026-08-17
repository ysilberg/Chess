cmake_minimum_required(VERSION 3.16)
project(Chess LANGUAGES CXX)

option(CHESS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(chess_core
    src/Board.cpp src/Piece.cpp src/Pawn.cpp src/Rook.cpp
    src/Knight.cpp src/Bishop.cpp src/Queen.cpp src/King.cpp)
target_include_directories(chess_core PUBLIC include)

function(enable_strict_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
        if(CHESS_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        if(CHESS_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

enable_strict_warnings(chess_core)

enable_testing()
add_executable(chess_tests tests/test_chess.cpp)
target_link_libraries(chess_tests PRIVATE chess_core)
enable_strict_warnings(chess_tests)
add_test(NAME chess_core_tests COMMAND chess_tests)

if(WIN32)
    add_executable(chess_app app/Source.cpp app/Manager.cpp app/stdafx.cpp)
    target_link_libraries(chess_app PRIVATE chess_core)
    target_include_directories(chess_app PRIVATE app)
    enable_strict_warnings(chess_app)
endif()
