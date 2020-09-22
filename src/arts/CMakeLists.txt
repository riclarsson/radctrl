########################################################################################
# ARTS Library: For ARTS usage
add_library (arts_interface STATIC
             interface.cpp)
target_link_libraries(arts_interface PRIVATE artscore methods matpack cdisort)
target_include_directories(arts_interface PRIVATE ${ARTS_BINARY_DIR}
                                                  ${ARTS_BINARY_DIR}/src/
                                                  ${ARTS_SOURCE_DIR}/src/
                                                  ${ARTS_SOURCE_DIR}/3rdparty/)
########################################################################################
