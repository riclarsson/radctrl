########################################################################################
# ARTS Library: For ARTS usage
add_library (arts_interface STATIC
  arts_agenda.cpp
  arts_atmosphere.cpp
  arts_continua.cpp
  arts_surface.cpp
  interface.cpp
)
target_link_libraries(arts_interface PUBLIC public_arts_interface)
########################################################################################

########################################################################################
# WASPAM forward and inverse simulations
add_executable(waspam_rt_calc waspam_water_simulations.cpp)
target_link_libraries(waspam_rt_calc PRIVATE arts_interface)
########################################################################################
