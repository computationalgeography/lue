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

add_library(lue_framework_io SHARED)
add_library(lue::framework_io ALIAS lue_framework_io)

set_target_properties(lue_framework_io
    PROPERTIES
        EXPORT_NAME framework_io
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

configure_file(
    include/lue/framework/io/configure.hpp.in
    include/lue/framework/io/configure.hpp
)

generate_export_header(lue_framework_io
    EXPORT_FILE_NAME include/lue/framework/io/export.hpp
    EXPORT_MACRO_NAME LUE_FRAMEWORK_IO_EXPORT
)

target_sources(lue_framework_io
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/io/configure.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/io/export.hpp
    PRIVATE
        source/dataset.cpp
        source/lue.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/source/gdal.cpp
)

target_link_libraries(lue_framework_io
    PRIVATE
        lue::framework_algorithm
        # HPX::iostreams_component
    PUBLIC
        lue::framework_partitioned_array
        lue::data_model_hl
        lue::gdal
        $<$<BOOL:${LUE_FRAMEWORK_WITH_PARALLEL_IO}>:MPI::MPI_CXX>
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_io
)

lue_install_development_libraries(
    TARGETS
        lue_framework_io
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
