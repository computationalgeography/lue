add_test_conditionally(test)

configure_file(
    include/configure.hpp.in
    include/lue/gdal/configure.hpp
)

configure_file(
    source/version.cpp.in
    source/version.cpp
)

add_library(lue_gdal STATIC
    source/blocks.cpp
    source/compare_rasters.cpp
    source/data_type.cpp
    source/dataset.cpp
    source/driver.cpp
    source/error.cpp
    source/raster.cpp
    source/raster_band.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/source/version.cpp
)

add_library(lue::gdal ALIAS lue_gdal)

set_target_properties(lue_gdal
    PROPERTIES
        EXPORT_NAME gdal
)

target_include_directories(lue_gdal
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_sources(lue_gdal
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/gdal/configure.hpp
)

# NOTE: Think twice before adding libraries here. lue::gdal is a simple, thin API around the
# GDAL API.
target_link_libraries(lue_gdal
    PUBLIC
        GDAL::GDAL
    PRIVATE
        fmt::fmt
)

lue_install_development_libraries(
    TARGETS lue_gdal
)
