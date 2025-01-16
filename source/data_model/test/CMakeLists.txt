add_library(lue_data_model_test STATIC
    source/dataset_fixture.cpp
    source/file_fixture.cpp
)

add_library(lue::data_model_test ALIAS lue_data_model_test)

target_include_directories(lue_data_model_test
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_data_model_test
    PUBLIC
        lue::data_model
)
