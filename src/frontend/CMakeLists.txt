add_library(frontend_impl STATIC
            frontend_dbr.cpp
            frontend_waspam.cpp
            )
target_link_libraries(frontend_impl PUBLIC gui python)
