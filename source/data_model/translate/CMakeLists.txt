add_test_conditionally(test)

add_library(lue_translate_lib STATIC
    src/stack_name.cpp
    src/translate.cpp

    src/command/export.cpp
    src/command/import.cpp

    src/format/dot.cpp
    src/format/gdal.cpp
    src/format/gdal_vector.cpp
    src/format/json.cpp
    src/format/lue.cpp
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
    src/main.cpp
)

target_link_libraries(lue_translate
    PRIVATE
        lue::translate
)

lue_install_executables(
    TARGETS lue_translate
)
