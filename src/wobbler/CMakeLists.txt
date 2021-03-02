add_library(wobbler_impl STATIC
            wobbler_iram_orig.cpp
            wobbler_waspam_orig.cpp
            )
target_link_libraries(wobbler_impl PUBLIC gui python)
