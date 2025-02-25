add_library(lue_data_model_hl SHARED
    source/dataset_view.cpp
    source/raster_view.cpp
    source/util.cpp
)

lue_configure_shared_library(
    TARGET_BASENAME data_model_hl
    EXPORT_HEADER_PATHNAME include/lue/data_model/hl/export.hpp
    EXPORT_MACRO_BASENAME DATA_MODEL_HL
)

target_link_libraries(lue_data_model_hl
    PUBLIC
        lue::data_model
)

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_data_model_hl
        TARGET_NAME_STATIC_LIB lue_data_model_hl_static
    )
    add_subdirectory(test)
endif()
