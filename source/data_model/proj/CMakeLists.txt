add_test_conditionally(test)

configure_file(
    include/configure.hpp.in
    include/lue/proj/configure.hpp
)

configure_file(
    src/version.cpp.in
    src/version.cpp
)

add_library(lue_proj STATIC
#     src/blocks.cpp
#     src/compare_rasters.cpp
#     src/data_type.cpp
#     src/dataset.cpp
#     src/driver.cpp
#     src/error.cpp
#     src/raster.cpp
#     src/raster_band.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/version.cpp
)

add_library(lue::proj ALIAS lue_proj)

set_target_properties(lue_proj
    PROPERTIES
        EXPORT_NAME proj
)

target_include_directories(lue_proj
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_sources(lue_proj
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/proj/configure.hpp
)

# NOTE: Think twice before adding libraries here. lue::proj is a simple, thin API around the
# PROJ API.
target_link_libraries(lue_proj
    PUBLIC
        PROJ::proj
    PRIVATE
        fmt::fmt
)

lue_install_development_libraries(
    TARGETS lue_proj
)
