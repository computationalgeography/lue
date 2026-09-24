add_library(lue_gdal SHARED)
add_library(lue::gdal ALIAS lue_gdal)

set_target_properties(lue_gdal
    PROPERTIES
        EXPORT_NAME gdal
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_gdal
    EXPORT_FILE_NAME include/lue/gdal/export.hpp
    EXPORT_MACRO_NAME LUE_GDAL_EXPORT
)

configure_file(
    include/configure.hpp.in
    include/lue/gdal/configure.hpp
)

configure_file(
    source/version.cpp.in
    source/version.cpp
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
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/gdal/export.hpp
    PRIVATE
        source/blocks.cpp
        source/compare_rasters.cpp
        source/data_type.cpp
        source/dataset.cpp
        source/driver.cpp
        source/error.cpp
        source/raster.cpp
        source/raster_band.cpp
        source/spatial_reference.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/source/version.cpp
)

# NOTE: Think twice before adding libraries here. lue::gdal is a simple, thin API around the GDAL API.
target_link_libraries(lue_gdal
    PUBLIC
        GDAL::GDAL
        lue::shared
)

lue_install_runtime_libraries(
    TARGETS
        lue_gdal
)

lue_install_development_libraries(
    TARGETS
        lue_gdal
)

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_gdal
        TARGET_NAME_STATIC_LIB lue_gdal_static
        EXPORT_MACRO_BASENAME GDAL
    )

    add_subdirectory(test)
endif()
