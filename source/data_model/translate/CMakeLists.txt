add_test_conditionally(test)

add_library(lue_translate_lib STATIC
    source/stack_name.cpp
    source/translate.cpp

    source/command/export.cpp
    source/command/import.cpp

    source/format/dot.cpp
    source/format/gdal.cpp
    source/format/gdal_vector.cpp
    source/format/json.cpp
    source/format/lue.cpp
)

add_library(lue::translate ALIAS lue_translate_lib)

target_include_directories(lue_translate_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_translate_lib
    PRIVATE
        lue::data_model_hl
    PUBLIC
        lue::utility
        lue::gdal
)

add_executable(lue_translate
    source/main.cpp
)

target_link_libraries(lue_translate
    PRIVATE
        lue::translate
)

lue_install_executables(
    TARGETS lue_translate
)
