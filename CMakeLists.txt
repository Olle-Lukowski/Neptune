cmake_minimum_required(VERSION 3.10)

project(Neptune)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(NP_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_definitions(NP_RELEASE)
endif()

add_library(Neptune 
  src/Neptune/Core.hpp
  src/Neptune/Core.cpp
  src/Neptune/Bitboard.hpp
  src/Neptune/Bitboard.cpp
  src/Neptune/Board.hpp
  src/Neptune/Board.cpp
  src/Neptune/Move.hpp
)

add_subdirectory(src/External/Catch2)

enable_testing()
add_executable(NeptuneTesting src/Testing.cpp src/Tests/Bitboard.cpp)

target_link_libraries(NeptuneTesting PRIVATE Neptune Catch2::Catch2WithMain)
target_include_directories(NeptuneTesting PRIVATE src src/External/Catch2/src)

add_test(NAME all COMMAND NeptuneTesting)

add_executable(NeptuneEngine
  src/Engine.cpp
)

target_link_libraries(NeptuneEngine PRIVATE Neptune)
