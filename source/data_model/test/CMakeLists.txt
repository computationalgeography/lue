add_library(lue_data_model_test STATIC
    source/dataset_fixture.cpp
    source/file_fixture.cpp
)

add_library(lue::data_model_test ALIAS lue_data_model_test)

target_include_directories(lue_data_model_test
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<TARGET_PROPERTY:lue::data_model,INCLUDE_DIRECTORIES>
)

target_compile_definitions(lue_data_model_test
    PRIVATE
        LUE_DATA_MODEL_STATIC_DEFINE
)








# target_link_libraries(lue_data_model_test
#     PUBLIC
#         lue::data_model
# )

# lue_configure_static_library_for_tests(
#     TARGET_NAME_SHARED_LIB lue_data_model_test
#     TARGET_NAME_STATIC_LIB lue_data_model_test_static
#     EXPORT_MACRO_BASENAME DATA_MODEL_TEST
# )
#
# target_link_libraries(lue_data_model_test_static
#     PUBLIC
#         lue_data_model_static
# )


# add_library(lue_data_model_test SHARED
#     source/dataset_fixture.cpp
#     source/file_fixture.cpp
# )
#
# lue_configure_shared_library(
#     TARGET_BASENAME data_model_test
#     EXPORT_HEADER_PATHNAME include/lue/test/export.hpp
#     EXPORT_MACRO_BASENAME DATA_MODEL_TEST
# )
#
# target_link_libraries(lue_data_model_test
#     PUBLIC
#         lue::data_model
# )
#
# lue_configure_static_library_for_tests(
#     TARGET_NAME_SHARED_LIB lue_data_model_test
#     TARGET_NAME_STATIC_LIB lue_data_model_test_static
#     EXPORT_MACRO_BASENAME DATA_MODEL_TEST
# )
#
# target_link_libraries(lue_data_model_test_static
#     PUBLIC
#         lue_data_model_static
# )
