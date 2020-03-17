add_test_conditionally(test)

add_library(lue_framework_model
    src/model
)

add_library(lue::framework_model ALIAS lue_framework_model)

target_include_directories(lue_framework_model
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_framework_model
    PUBLIC
        lue::framework_core
)
