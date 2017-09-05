add_library(lue_translate_lib STATIC
    src/translate

    src/command/export
    src/command/import

    src/format/csv
    src/format/dot
    src/format/gdal
    src/format/gdal_block
    src/format/lue
    src/format/vtk
)

add_library(lue::translate ALIAS lue_translate_lib)

target_include_directories(lue_translate_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_translate_lib
    PRIVATE
        lue::hl
        ${GDAL_LIBRARIES}
    PUBLIC
        lue::utility
)

add_executable(lue_translate
    src/main
)

target_link_libraries(lue_translate
    PRIVATE
        lue::translate
)

install(
    TARGETS lue_translate
    RUNTIME DESTINATION bin
)
