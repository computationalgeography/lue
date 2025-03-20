add_library(lue_data_model_hl SHARED)
add_library(lue::data_model_hl ALIAS lue_data_model_hl)

generate_export_header(lue_data_model_hl
    EXPORT_FILE_NAME include/lue/data_model_hl/export.hpp
    EXPORT_MACRO_NAME LUE_HDF5_EXPORT
)

target_sources(lue_data_model_hl
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/data_model_hl/export.hpp
    PRIVATE
        source/dataset_view.cpp
        source/raster_view.cpp
        source/util.cpp
)

target_link_libraries(lue_data_model_hl
    PUBLIC
        lue::data_model
)

lue_install_runtime_libraries(
    TARGETS
        lue_data_model_hl
)

lue_install_development_libraries(
    TARGETS
        lue_data_model_hl
)

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_data_model_hl
        TARGET_NAME_STATIC_LIB lue_data_model_hl_static
        EXPORT_MACRO_BASENAME DATA_MODEL_HL
    )

    add_subdirectory(test)
endif()
