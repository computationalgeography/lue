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

add_library(lue_framework_io
    source/dataset.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/source/gdal.cpp
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
        lue::gdal
)
