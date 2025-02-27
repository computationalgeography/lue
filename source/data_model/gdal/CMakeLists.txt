configure_file(
    include/configure.hpp.in
    include/lue/gdal/configure.hpp
)

configure_file(
    source/version.cpp.in
    source/version.cpp
)

add_library(lue_gdal SHARED
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

lue_configure_shared_library(
    TARGET_BASENAME gdal
    EXPORT_HEADER_PATHNAME include/lue/gdal/export.hpp
    EXPORT_MACRO_BASENAME GDAL
)

target_sources(lue_gdal
    PUBLIC
        FILE_SET
            HEADERS
        FILES
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

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_gdal
        TARGET_NAME_STATIC_LIB lue_gdal_static
        EXPORT_MACRO_BASENAME GDAL
    )
    add_subdirectory(test)
endif()
