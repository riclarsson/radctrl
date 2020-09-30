include (FindOpenMP)
if (NOT ${OPENMP_FOUND})
  message(FATAL_ERROR "No OPEN MP found, ARTS requires it")
endif()

########################################################################################
# ARTS API STATIC: For ARTS usage
add_library (arts_api_static STATIC ${ARTS_SOURCE_DIR}/src/arts.cc)
target_link_libraries (arts_api_static artscore methods matpack cdisort OpenMP::OpenMP_CXX Faddeeva)  # FIXME: Add other non-default packages...
target_include_directories(arts_api_static PUBLIC ${ARTS_BINARY_DIR}
                                                  ${ARTS_BINARY_DIR}/src/
                                                  ${ARTS_SOURCE_DIR}/src/
                                                  ${ARTS_SOURCE_DIR}/3rdparty/
                                                  ${ARTS_SOURCE_DIR}/3rdparty/invlib/src)
########################################################################################
                                                   
########################################################################################
# ARTS Library: For ARTS usage
add_library (arts_interface STATIC interface.cpp arts_agenda.cpp autoarts.h)
target_link_libraries(arts_interface PUBLIC arts_api_static)
target_include_directories(arts_interface PUBLIC ${RADCALC_BINARY_DIR}/src/arts/)
########################################################################################

########################################################################################
# Generate the internal structure for the ARTS interface
add_executable(genautoarts genautoarts.cpp)
target_link_libraries(genautoarts PUBLIC arts_api_static)
########################################################################################

########################################################################################
# Generate autoarts.h for the ARTS interface
add_custom_command(OUTPUT autoarts.h
                   COMMAND genautoarts > ${RADCALC_BINARY_DIR}/src/arts/autoarts.h
                   DEPENDS genautoarts)
########################################################################################
