add_test_conditionally(test)

add_library(lue_framework_io
    src/read.cpp
    src/util.cpp
    src/write.cpp
)

add_library(lue::framework_io ALIAS lue_framework_io)

target_include_directories(lue_framework_io
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_framework_io
    PUBLIC
        lue::framework_algorithm
        lue::data_model_hl
)
