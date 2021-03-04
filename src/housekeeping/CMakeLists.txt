add_library(housekeeping_impl STATIC
            housekeeping_agilent.cpp
            housekeeping_pysensors.cpp
            )
target_link_libraries(housekeeping_impl PUBLIC gui python network)
