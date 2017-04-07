set(SOURCES
    dot
    gdal
    gdal_block
    lue
)
add_library(lue_translate_format_objects OBJECT
    ${SOURCES}
)
