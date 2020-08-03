cmake_minimum_required (VERSION 3.9.6)
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message (FATAL_ERROR
"You are not allowed to build from this folder"
)
endif ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

project (RADCALC CXX)
enable_language(CXX)

add_subdirectory(inc)
add_subdirectory(python)
add_subdirectory(src)
