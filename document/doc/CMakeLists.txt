find_package(Doxygen REQUIRED)

set(DOXYGEN_INPUT_DIR
        ${PROJECT_SOURCE_DIR}/source/data_model/hdf5
    )
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)

configure_file(Doxyfile.in Doxyfile @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

add_custom_command(
    OUTPUT ${DOXYGEN_INDEX_FILE}
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    MAIN_DEPENDENCY Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

add_custom_target(doxygen_doc DEPENDS ${DOXYGEN_INDEX_FILE})


find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD_DIR}/index.html)

file(GLOB_RECURSE sphinx_sources LIST_DIRECTORIES False *.rst)

add_custom_command(
    OUTPUT ${SPHINX_INDEX_FILE}
    COMMAND ${SPHINX_BUILD_EXECUTABLE} -b html
        -Dbreathe_projects.LUE=${DOXYGEN_OUTPUT_DIR}/xml
        ${SPHINX_SOURCE_DIR} ${SPHINX_BUILD_DIR}
        DEPENDS
            ${sphinx_sources}
            ${DOXYGEN_INDEX_FILE}
        MAIN_DEPENDENCY
            ${CMAKE_CURRENT_SOURCE_DIR}/conf.py)

add_custom_target(sphinx_doc DEPENDS ${SPHINX_INDEX_FILE})

install(
    DIRECTORY ${SPHINX_BUILD_DIR}
    DESTINATION ${CMAKE_INSTALL_DOCDIR})
