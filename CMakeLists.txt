cmake_minimum_required(VERSION 3.16)
project(rumo_a_liga_pokemon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(rumo_liga src/main.cpp)
target_include_directories(rumo_liga PRIVATE headings)
target_compile_options(rumo_liga PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_executable(testes tests/testes.cpp)
target_include_directories(testes PRIVATE headings)
target_compile_options(testes PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME testes COMMAND testes)
set_tests_properties(testes PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
