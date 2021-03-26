add_library(lue_data_model_test STATIC
    src/dataset_fixture.cpp
    src/file_fixture.cpp
)

add_library(lue::data_model_test ALIAS lue_data_model_test)

target_include_directories(lue_data_model_test
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_data_model_test
    PUBLIC
        lue::data_model
)
