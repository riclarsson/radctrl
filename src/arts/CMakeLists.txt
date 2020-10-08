########################################################################################
# ARTS Library: For ARTS usage
add_library (arts_interface STATIC interface.cpp arts_agenda.cpp)
target_link_libraries(arts_interface PUBLIC public_arts_interface)
target_compile_options(arts_interface PRIVATE -Wno-odr -Wno-deprecated_copy)
########################################################################################
