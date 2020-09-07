# Global compile time options
add_compile_options(-Wall -Wextra -Wdouble-promotion -Wimplicit-fallthrough=5 -Wno-ignored-attributes -O3 -march=native -fvisibility=hidden -flto)

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
target_include_directories(gui PUBLIC ../3rdparty/gui/imgui/ ../3rdparty/gui/implot/ ../3rdparty/gui/imgui-filebrowser/)
########################################################################################

########################################################################################
# CLI parsing: For parsing argc and argv nicely
add_library (cli STATIC cli_parsing.cpp cli_runtime.cpp)
target_include_directories(cli PUBLIC ../3rdparty/cmd/CLI11/include/CLI/ ../3rdparty/cmd/cli/include/cli/)

########################################################################################

########################################################################################
# Math Library: For basic mathematical constructs
add_library (math STATIC
             mathhelpers.cpp
             rational.cpp
             wigner.cpp
             openblas_interface.cpp
             )
target_include_directories(math PUBLIC ../3rdparty/Eigen/ ../3rdparty/wigner/ ../3rdparty/Faddeeva/)
target_link_libraries(math PUBLIC wigner Faddeeva openblas lapack kernel)
if(FASTWIGNER)
target_compile_definitions(math PUBLIC "-DFAST_WIGNER_PATH_3J=\"${RADCALC_BINARY_DIR}/3rdparty/wigner/fastwigxj/fasttable.3j\"")
target_compile_definitions(math PUBLIC "-DFAST_WIGNER_PATH_6J=\"${RADCALC_BINARY_DIR}/3rdparty/wigner/fastwigxj/fasttable.6j\"")
endif()

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
target_include_directories(files PUBLIC ../3rdparty/pugixml/)
########################################################################################

########################################################################################
# Atmospheric Library:  For equations and classes for computing the atmosphere
add_library (atm STATIC
             atm.cpp
             )
target_link_libraries(atm PUBLIC files)
########################################################################################

########################################################################################
# ASIO Interface
add_library (network STATIC
             asio_interface.cpp
             )
target_include_directories(network PUBLIC ../3rdparty/asio/asio/include/)
target_link_libraries(network PUBLIC multithread)
########################################################################################

########################################################################################
# Chopper library
add_library (chopper STATIC
             chopper.cpp
             )
target_link_libraries(chopper PUBLIC gui python cli)
add_executable (chopper_bin chopper.cpp)
target_link_libraries(chopper_bin PUBLIC chopper)
set_target_properties(chopper_bin PROPERTIES OUTPUT_NAME "chopper")
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
target_link_libraries(frontend PUBLIC gui python cli)
add_executable (frontend_bin frontend.cpp)
target_link_libraries(frontend_bin PUBLIC frontend)
set_target_properties(frontend_bin PROPERTIES OUTPUT_NAME "frontend")
########################################################################################

########################################################################################
# Housekeeping library
add_library (housekeeping STATIC
             housekeeping.cpp
             )
target_link_libraries(housekeeping PUBLIC gui python network cli)
add_executable (housekeeping_bin housekeeping.cpp)
target_link_libraries(housekeeping_bin PUBLIC housekeeping)
set_target_properties(housekeeping_bin PROPERTIES OUTPUT_NAME "housekeeping")
########################################################################################

########################################################################################
# Geometry Library:  For equations and classes for computing the path and geometry
add_library (geom STATIC
             geom.cpp
             )
target_link_libraries(geom PUBLIC species files)
########################################################################################


########################################################################################
# Atmospheric Pathing Library
add_library (atmpath STATIC
             atmpath.cpp
             )
target_link_libraries(atmpath PUBLIC atm geom)
########################################################################################

########################################################################################
# Cross-section line-by-line library:  For computing cross-sections of absorption lines
add_library (xsec_lbl STATIC
             xsec_lbl.cpp
             lineshapes.cpp
             lbl.cpp
             zeeman.cpp
             )
target_link_libraries(xsec_lbl PUBLIC atmpath multithread math)
########################################################################################

########################################################################################
# Cross-section library:  For computing cross-sections of any kind
add_library (xsec STATIC
             xsec.cpp
             )
target_link_libraries(xsec PUBLIC xsec_lbl)
########################################################################################

########################################################################################
# Absorption Library:  For equations and classes for computing the Propagation Matrix
add_library (absorption STATIC
             propmat.cpp
             )
target_link_libraries(absorption PUBLIC species files xsec)
########################################################################################

########################################################################################
# Cross-section library:  For computing cross-sections of any kind
add_library (forward STATIC
             forward.cpp
             )
target_link_libraries(forward PUBLIC absorption rte)
########################################################################################

########################################################################################
# Radiative Transfer Library:  For equations and classes for computing the path and geometry
add_library (rte STATIC
             rte.cpp
             )
target_link_libraries(rte PUBLIC species absorption)
########################################################################################

########################################################################################
# Python Interface Library:  For calling python code
set(Python_ADDITIONAL_VERSIONS 3)
add_library(python STATIC python_interface.cpp)
target_include_directories(python PUBLIC ${PYTHON_INCLUDE_DIRS} ../3rdparty/pybind11/include/ ../3rdparty/Eigen/)
target_link_libraries(python PUBLIC ${PYTHON_LIBRARIES} pybind11::embed)
########################################################################################

########################################################################################
# Multithreading interfaces
find_package (Threads REQUIRED)
find_package (TBB REQUIRED)
add_library (multithread STATIC multithread.cpp)
target_link_libraries(multithread PUBLIC Threads::Threads tbb)
########################################################################################

########################################################################################
# Instrument GUI libraries
add_library (instrument_gui_controller STATIC dummy_gui.cpp)
target_link_libraries(instrument_gui_controller PUBLIC gui python files chopper wobbler housekeeping backend multithread frontend cli)
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
# Test atmosphere interface
add_executable(test_atm test_atm.cpp)
target_link_libraries(test_atm PUBLIC atm)
########################################################################################

########################################################################################
# Test GUI interface
add_executable(test_gui test_gui.cpp)
target_link_libraries(test_gui PUBLIC gui)
########################################################################################

########################################################################################
# Test atmosphere pathing interface
add_executable(test_atmpath test_atmpath.cpp)
target_link_libraries(test_atmpath PUBLIC atmpath)
########################################################################################

########################################################################################
# Test lbl calculations
add_executable(test_xsec_lbl test_xsec_lbl.cpp)
target_link_libraries(test_xsec_lbl PUBLIC xsec_lbl)
########################################################################################

########################################################################################
# Test xsec calculations
add_executable(test_xsec test_xsec.cpp)
target_link_libraries(test_xsec PUBLIC xsec)
########################################################################################

########################################################################################
# Tests to understand std::filesystem
add_executable(test_filesystem test_filesystem.cpp)
########################################################################################

########################################################################################
# Tests to understand Grids
add_executable(test_grids test_grids.cpp)
########################################################################################

########################################################################################
# Tests to understand OpenBLAS
add_executable(test_blas test_blas.cpp)
target_link_libraries(test_blas PUBLIC math)
########################################################################################

########################################################################################
# Tests to understand Forward
add_executable(test_forward test_forward.cpp)
target_link_libraries(test_forward PUBLIC forward)
########################################################################################
