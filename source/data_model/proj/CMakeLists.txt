add_library(lue_proj SHARED)
add_library(lue::proj ALIAS lue_proj)

set_target_properties(lue_proj
    PROPERTIES
        EXPORT_NAME proj
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_proj
    EXPORT_FILE_NAME include/lue/proj/export.hpp
    EXPORT_MACRO_NAME LUE_PROJ_EXPORT
)

configure_file(
    include/configure.hpp.in
    include/lue/proj/configure.hpp
)

configure_file(
    source/version.cpp.in
    source/version.cpp
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
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/proj/export.hpp
    PRIVATE
        # source/blocks.cpp
        # source/compare_rasters.cpp
        # source/data_type.cpp
        # source/dataset.cpp
        # source/driver.cpp
        # source/error.cpp
        # source/raster.cpp
        # source/raster_band.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/source/version.cpp
)

# NOTE: Think twice before adding libraries here. lue::proj is a simple, thin API around the PROJ API.
target_link_libraries(lue_proj
    PUBLIC
        PROJ::proj
        lue::shared
)

lue_install_runtime_libraries(
    TARGETS
        lue_proj
)

lue_install_development_libraries(
    TARGETS
        lue_proj
)

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_proj
        TARGET_NAME_STATIC_LIB lue_proj_static
        EXPORT_MACRO_BASENAME PROJ
    )

    add_subdirectory(test)
endif()
