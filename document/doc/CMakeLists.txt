if(LUE_BUILD_DATA_MODEL)
    set(data_model_root "${PROJECT_SOURCE_DIR}/source/data_model")

    list(APPEND DOXYGEN_INPUT_DIR
        ${data_model_root}/cxx
        ${data_model_root}/doc
        ${data_model_root}/hdf5
        ${data_model_root}/hl)

    if(LUE_DATA_MODEL_WITH_UTILITIES)
        list(APPEND DOXYGEN_INPUT_DIR
            ${data_model_root}/translate
            ${data_model_root}/utility
            ${data_model_root}/validate)
    endif()

    if(LUE_DATA_MODEL_WITH_PYTHON_API)
        list(APPEND DOXYGEN_INPUT_DIR
            ${data_model_root}/python)
    endif()
endif()


set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_OUTPUT_DIRECTORY doxygen)

doxygen_add_docs(
        doxygen_doc
        ${DOXYGEN_INPUT_DIR}
    )

set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_OUTPUT_DIRECTORY})


set(SPHINX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD_DIR}/index.html)

file(GLOB_RECURSE sphinx_sources LIST_DIRECTORIES False *.rst)

configure_file(conf.py.in conf.py @ONLY)

add_custom_command(
    OUTPUT ${SPHINX_INDEX_FILE}
    COMMAND
        ${CMAKE_COMMAND} -E env "PYTHONPATH=${PROJECT_BINARY_DIR}/lib:$ENV{PYTHONPATH}"
            ${SPHINX_BUILD_EXECUTABLE} -b html
                -c ${CMAKE_CURRENT_BINARY_DIR}
                -Dbreathe_projects.LUE=${DOXYGEN_OUTPUT_DIR}/xml
                ${SPHINX_SOURCE_DIR} ${SPHINX_BUILD_DIR}
        DEPENDS
            doxygen_doc
            ${sphinx_sources}
        MAIN_DEPENDENCY
            ${CMAKE_CURRENT_BINARY_DIR}/conf.py)

add_custom_target(sphinx_doc DEPENDS ${SPHINX_INDEX_FILE})

# TODO Make this depend on the generation of the docs. They must be ready to copy.
# install(
#     DIRECTORY
#         ${SPHINX_BUILD_DIR}
#     DESTINATION ${CMAKE_INSTALL_DOCDIR}
#     COMPONENT lue_development
# )
