add_test_conditionally(test)


# configure_file(
#     include/lue/netcdf4/configure.hpp.in
#     include/lue/netcdf4/configure.hpp
# )

add_library(lue_data_model_netcdf4 STATIC
    src/attribute.cpp
    src/dimension.cpp
    src/file.cpp
    src/group.cpp
    src/identifier.cpp
    src/util.cpp
    src/variable.cpp
)

add_library(lue::data_model::netcdf4 ALIAS lue_data_model_netcdf4)

target_include_directories(lue_data_model_netcdf4
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        # $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_data_model_netcdf4
    PUBLIC
        NetCDF::NetCDF
    PRIVATE
        fmt::fmt
)
