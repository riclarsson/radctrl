# Global compile time options
add_compile_options(-Wall -Wextra -pedantic -O3 -march=native -fvisibility=hidden -flto)

# Sub directories

########################################################################################
# GUI Library: For basic GUI creation
add_library (gui STATIC
             gui.cpp
             gui_menubar.cpp
             gui_help.cpp
             gui_plotting.cpp
             )
target_link_libraries(gui PUBLIC imgui implot)
target_include_directories(gui PUBLIC ../inc/gui/imgui/ ../inc/gui/implot/ ../inc/gui/imgui-filebrowser/)
########################################################################################

########################################################################################
# Math Library: For basic mathematical constructs
add_library (math STATIC
             mathhelpers.cpp
             rational.cpp
             )
target_include_directories(math PUBLIC ../inc/Eigen/)
########################################################################################

########################################################################################
# Species Library:  For equations and classes for computing the atmosphere
add_library (species STATIC
             species.cpp
             hitran.cpp
             )
target_link_libraries(species PUBLIC math)
########################################################################################

########################################################################################
# Atmospheric Library:  For equations and classes for computing the atmosphere
add_library (files STATIC
             file.cpp
             xml_config.cpp
             )
target_link_libraries(files PUBLIC xml)
target_include_directories(files PUBLIC ../inc/pugixml/)
########################################################################################

########################################################################################
# Atmospheric Library:  For equations and classes for computing the atmosphere
add_library (atm STATIC
             atm.cpp
             )
########################################################################################

########################################################################################
# Chopper library
add_library (chopper STATIC
             chopper.cpp
             )
target_link_libraries(chopper PUBLIC gui python)
########################################################################################

########################################################################################
# Wobbler library
add_library (wobbler STATIC
             wobbler.cpp
             )
target_link_libraries(wobbler PUBLIC gui python)
########################################################################################

########################################################################################
# Backend library
add_library (backend STATIC
             backend.cpp
             )
target_link_libraries(backend PUBLIC files gui python math)
########################################################################################

########################################################################################
# Frontend library
add_library (frontend STATIC
             frontend.cpp
             )
target_link_libraries(frontend PUBLIC gui python)
########################################################################################

########################################################################################
# Housekeeping library
add_library (housekeeping STATIC
             housekeeping.cpp
             )
target_link_libraries(housekeeping PUBLIC gui python)
########################################################################################

########################################################################################
# Geometry Library:  For equations and classes for computing the path and geometry
add_library (geom STATIC
             geom.cpp
             )
target_link_libraries(geom PUBLIC species files)
########################################################################################

########################################################################################
# Absorption Library:  For equations and classes for computing the path and geometry
add_library (absorption STATIC
             propmat.cpp
             lbl.cpp
             lineshapes.cpp
             )
target_include_directories(absorption PUBLIC ../inc/Faddeeva/)
target_link_libraries(absorption PUBLIC species files)
########################################################################################

########################################################################################
# Radiative Transfer Library:  For equations and classes for computing the path and geometry
add_library (rte STATIC
             rte.cpp
             )
target_link_libraries(rte PUBLIC species)
########################################################################################

########################################################################################
# Python Interface Library:  For calling python code
set(Python_ADDITIONAL_VERSIONS 3)
add_library(python STATIC python_interface.cpp)
target_include_directories(python PUBLIC ${PYTHON_INCLUDE_DIRS} ../inc/pybind11/include/ ../inc/Eigen/)
target_link_libraries(python PUBLIC ${PYTHON_LIBRARIES} pybind11::embed)
########################################################################################

########################################################################################
# Multithreading interfaces
find_package (Threads REQUIRED)
add_library (multithread STATIC multithread.cpp)
target_link_libraries(multithread PUBLIC Threads::Threads)
########################################################################################

########################################################################################
# Instrument GUI libraries
add_library (instrument_gui_controller STATIC dummy_gui.cpp)
target_link_libraries(instrument_gui_controller PUBLIC gui python files chopper wobbler housekeeping backend multithread frontend)
########################################################################################

########################################################################################
# Dummy GUI
add_executable (dummy_gui dummy_gui.cpp)
target_link_libraries(dummy_gui PUBLIC instrument_gui_controller)
install (TARGETS dummy_gui RUNTIME DESTINATION bin)
########################################################################################

########################################################################################
# Waspam runtime executable
add_executable (waspam waspam.cpp)
target_link_libraries(waspam PUBLIC instrument_gui_controller)
install (TARGETS waspam RUNTIME DESTINATION bin)
########################################################################################

########################################################################################
# IRAM runtime executable
add_executable (iram iram.cpp)
target_link_libraries(iram PUBLIC instrument_gui_controller)
install (TARGETS iram RUNTIME DESTINATION bin)
########################################################################################

########################################################################################

########################################################################################
# Test multithreading
add_executable(test_multithread test_multithread.cpp)
target_link_libraries(test_multithread PUBLIC multithread)
########################################################################################

########################################################################################
# Test geometry interface
add_executable(test_geom test_geom.cpp)
target_link_libraries(test_geom PUBLIC geom)
########################################################################################

########################################################################################
# Test RTE interface
add_executable(test_rte test_rte.cpp)
target_link_libraries(test_rte PUBLIC rte)
########################################################################################

########################################################################################
# Test files interface
add_executable(test_file test_file.cpp)
target_link_libraries(test_file PUBLIC files)
########################################################################################

########################################################################################
# Test HITRAN interface
add_executable(test_hitran test_hitran.cpp)
target_link_libraries(test_hitran PUBLIC absorption files)
########################################################################################

########################################################################################
# Test python interface
add_executable(test_python test_python.cpp)
target_link_libraries(test_python PUBLIC python)
########################################################################################

########################################################################################
# Test GUI interface
add_executable(test_gui test_gui.cpp)
target_link_libraries(test_gui PUBLIC gui)
########################################################################################

########################################################################################
# Tests to understand std::filesystem
add_executable(test_filesystem test_filesystem.cpp)
########################################################################################
