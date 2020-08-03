add_compile_options(-Wall -Wextra -pedantic -O0)
            
# GUI library
add_library (gui STATIC
             gui.cpp
             gui_menubar.cpp
             gui_help.cpp
             )
target_link_libraries(gui imgui implot)
target_include_directories(gui PUBLIC ../inc/gui/imgui/)
target_include_directories(gui PUBLIC ../inc/gui/implot/)

# Species Library:  For equations and classes for computing the atmosphere
add_library (species STATIC
             species.cpp
             hitran.cpp
             rational.cpp
             )
            
# Atmospheric Library:  For equations and classes for computing the atmosphere
add_library (files STATIC
             file.cpp
             )
target_link_libraries(files xml)
            
# Atmospheric Library:  For equations and classes for computing the atmosphere
add_library (atm STATIC
             atm.cpp
             )
            
# Chopper library
add_library (chopper STATIC
             chopper.cpp
             )
target_link_libraries(chopper gui)
            
# Geometry Library:  For equations and classes for computing the path and geometry
add_library (geom STATIC
             geom.cpp
             )
target_link_libraries(geom species files)
            
# Absorption Library:  For equations and classes for computing the path and geometry
add_library (absorption STATIC
             propmat.cpp
             lbl.cpp
             lineshapes.cpp
             )
target_include_directories(absorption PUBLIC ../inc/Faddeeva/)
target_link_libraries(absorption species files)
            
# Radiative Transfer Library:  For equations and classes for computing the path and geometry
add_library (rte STATIC
             rte.cpp
             )
target_link_libraries(rte species)
            
# Python Interface Library:  For calling python code
set(Python_ADDITIONAL_VERSIONS 3)
find_package (PythonInterp REQUIRED)
find_package (PythonLibs REQUIRED)
add_library(python python_interface.cpp)
include_directories(${PYTHON_INCLUDE_DIRS})
message("-- Linking: ${PYTHON_INCLUDE_DIRS}")
message("-- Linking: ${PYTHON_LIBRARIES}")
include_directories(../inc/pybind11/include/)
target_link_libraries(python PUBLIC ${PYTHON_LIBRARIES})
target_link_libraries(python PUBLIC pybind11::embed)

add_executable (waspam waspam.cpp)
target_link_libraries(waspam gui python files chopper)

add_executable (iram iram.cpp)
target_link_libraries(iram gui python files chopper)

find_package (Threads REQUIRED)
# Lineshapes Library:  For equations and classes for computing the line shapes
add_library (multithread multithread.cpp)
target_link_libraries(multithread PUBLIC Threads::Threads)

add_executable(test_multithread test_multithread.cpp)
target_link_libraries(test_multithread multithread)

#### TESTS ###
add_executable(test_geom test_geom.cpp)
target_link_libraries(test_geom geom)

add_executable(test_rte test_rte.cpp)
target_link_libraries(test_rte rte)

add_executable(test_file test_file.cpp)
target_link_libraries(test_file files)

add_executable(test_hitran test_hitran.cpp)
target_link_libraries(test_hitran absorption files)

add_executable(test_python test_python.cpp)
target_link_libraries(test_python python)

add_executable(test_gui test_gui.cpp)
target_link_libraries(test_gui gui)

add_executable(test_filesystem test_filesystem.cpp)


