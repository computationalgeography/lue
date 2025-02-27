add_test_conditionally(test)

block()
    set(Elements ${LUE_FRAMEWORK_ELEMENTS})
    list(TRANSFORM Elements PREPEND "\"")
    list(TRANSFORM Elements APPEND "\"")
    list(JOIN Elements ", " Elements)

    generate_template_instantiation(
        INPUT_PATHNAME
            "${CMAKE_CURRENT_SOURCE_DIR}/source/gdal.cpp.in"
        OUTPUT_PATHNAME
            "${CMAKE_CURRENT_BINARY_DIR}/source/gdal.cpp"
        DICTIONARY
            '{"Elements":[${Elements}],"ranks":[${LUE_FRAMEWORK_RANKS}]}'
    )
endblock()

add_library(lue_framework_io SHARED
    source/dataset.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/source/gdal.cpp
)

lue_configure_shared_library(
    TARGET_BASENAME framework_io
    EXPORT_HEADER_PATHNAME include/lue/framework/io/export.hpp
    EXPORT_MACRO_BASENAME FRAMEWORK_IO
)

target_link_libraries(lue_framework_io
    PRIVATE
        lue::framework_algorithm
    PUBLIC
        lue::framework_partitioned_array
        lue::data_model_hl
        lue::gdal
        $<$<BOOL:${LUE_FRAMEWORK_WITH_PARALLEL_IO}>:MPI::MPI_CXX>
)
