add_library(backend_impl STATIC
            backend_rcts104.cpp
            backend_swicts.cpp
            backend_pc104.cpp
            backend_affts.cpp
            backend_dffts.cpp
            backend_xffts.cpp
            )
target_link_libraries(backend_impl PUBLIC math gui python)
