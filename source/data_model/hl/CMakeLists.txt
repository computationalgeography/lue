add_test_conditionally(test)

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
