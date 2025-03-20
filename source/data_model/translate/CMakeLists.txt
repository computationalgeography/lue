add_library(lue_translate_lib STATIC)
add_library(lue::translate ALIAS lue_translate_lib)

target_sources(lue_translate_lib
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/lue
    PRIVATE
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

target_link_libraries(lue_translate_lib
    PUBLIC
        lue::utility
        lue::gdal
    PRIVATE
        lue::data_model_hl
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

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
