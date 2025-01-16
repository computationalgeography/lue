add_test_conditionally(test)

add_library(lue_data_model_hl
    source/dataset_view.cpp
    source/raster_view.cpp
    source/util.cpp
)

add_library(lue::data_model_hl ALIAS lue_data_model_hl)

target_include_directories(lue_data_model_hl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_data_model_hl
    PUBLIC
        lue::data_model
)
