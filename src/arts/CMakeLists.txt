include (FindOpenMP)
if (NOT ${OPENMP_FOUND})
  message(FATAL_ERROR "No OPEN MP found, ARTS requires it")
endif()

########################################################################################
# ARTS Library: For ARTS usage
add_library (arts_interface STATIC interface.cpp arts_agenda.cpp)
target_link_libraries(arts_interface PUBLIC public_arts_interface OpenMP::OpenMP_CXX)
########################################################################################
