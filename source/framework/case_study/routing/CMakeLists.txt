# add_library(lue_routing
#     src/routing_model_base.cpp
# )
# 
# add_library(lue::routing ALIAS lue_routing)
# 
# target_link_libraries(lue_routing
#     PRIVATE
#         lue::framework_algorithm
#         lue::framework_model)

### hpx_setup_target(lue_routing
###     TYPE LIBRARY
###     COMPONENT_DEPENDENCIES
###         iostreams
### )

# target_include_directories(lue_routing
#     PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
# )

### target_link_libraries(lue_routing
###     PUBLIC
###         lue::framework_algorithm
###         lue::framework_io
###         lue::framework_model
###         lue::benchmark
### )

add_subdirectory(benchmark)

### add_hpx_executable(lue_burn_wilderness
###     ESSENTIAL
###     SOURCES
###         src/main.cpp
### )
### 
### target_link_libraries(lue_burn_wilderness
###     PRIVATE
###         lue::routing
### )
