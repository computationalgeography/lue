add_subdirectory(case_study)

configure_file(
    conf.py.in
    _build/conf.py
    @ONLY
)

add_custom_target(
    py_doc
    DEPENDS
        case_study
        lue::py
    # COMMAND
    #     ${SPHINX_APIDOC_EXECUTABLE}
    #         -o ${CMAKE_CURRENT_BINARY_DIR}/_build/api
    #         --separate
    #         ${PROJECT_SOURCE_DIR}/source
    COMMAND ${CMAKE_COMMAND} -E
        env PYTHONPATH=$<TARGET_FILE_DIR:lue::py>
        ${SPHINX_BUILD_EXECUTABLE}
            -q -b html
            -c ${CMAKE_CURRENT_BINARY_DIR}/_build
            -d ${CMAKE_CURRENT_BINARY_DIR}/_build/doctrees
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}/_build/html
    COMMENT "Generating luepy documentation"
)
