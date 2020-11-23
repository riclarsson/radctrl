########################################################################################
# ARTS Library: For ARTS usage
add_executable (plot plot.cpp)
target_link_libraries(plot PUBLIC cli gui files multithread)
target_include_directories(plot PUBLIC ..)
########################################################################################
