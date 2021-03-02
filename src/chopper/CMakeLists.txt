add_library(chopper_impl STATIC
            chopper_orig.cpp
            )
target_link_libraries(chopper_impl PUBLIC gui python)
