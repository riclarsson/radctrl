cmake_minimum_required (VERSION 3.9.6)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

cmake_policy(SET CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

project (RADCALC CXX C Fortran)
enable_language(CXX C Fortran)
include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# enable cache system
include(cmake/Cache.cmake)

# enable doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

#enable latex

include(cmake/UseLATEX.cmake)

add_subdirectory(3rdparty)
add_subdirectory(python)
add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(doc)

