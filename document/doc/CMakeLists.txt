set(LUE_TITLE "LUE documentation")
set(LUE_AUTHOR "Computational Geography R&D Team")
set(LUE_BOOK_BASENAME "lue-${CMAKE_PROJECT_VERSION}")

set(DOXYGEN_HAVE_DOT YES)
set(DOXYGEN_DOT_MULTI_TARGETS YES)
set(DOXYGEN_EXCLUDE_PATTERNS
    */*.py
)
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_OUTPUT_DIRECTORY doxygen)
set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)

doxygen_add_docs(
        doxygen_doc
        ${PROJECT_SOURCE_DIR}/source
    )

set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_OUTPUT_DIRECTORY})
# set(AUTODOC2_PACKAGE_DIR ../../${LUE_BUILD_PYTHON_PACKAGE_DIR})


# set(SPHINX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# set(SPHINX_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
# set(SPHINX_INDEX_FILE ${SPHINX_BUILD_DIR}/index.html)
#
# file(GLOB_RECURSE sphinx_sources LIST_DIRECTORIES False *.rst)
#
# configure_file(conf.py.in conf.py @ONLY)
#
# add_custom_command(
#     OUTPUT ${SPHINX_INDEX_FILE}
#     COMMAND
#         ${CMAKE_COMMAND}
#             -E env "PYTHONPATH=${PROJECT_BINARY_DIR}/${LUE_INSTALL_PYTHON_PACKAGE_DIR}/..:$ENV{PYTHONPATH}"
#             ${SPHINX_BUILD_EXECUTABLE} -b html
#                 -c ${CMAKE_CURRENT_BINARY_DIR}
#                 -Dbreathe_projects.LUE=${DOXYGEN_OUTPUT_DIR}/xml
#                 ${SPHINX_SOURCE_DIR} ${SPHINX_BUILD_DIR}
#         DEPENDS
#             doxygen_doc
#             ${sphinx_sources}
#         MAIN_DEPENDENCY
#             ${CMAKE_CURRENT_BINARY_DIR}/conf.py)
#
# add_custom_target(sphinx_doc DEPENDS ${SPHINX_INDEX_FILE})
#
# # TODO Make this depend on the generation of the docs. They must be ready to copy.
# # install(
# #     DIRECTORY
# #         ${SPHINX_BUILD_DIR}
# #     DESTINATION ${CMAKE_INSTALL_DOCDIR}
# #     COMPONENT lue_development
# # )


# # https://www.sphinx-doc.org/en/master/man/sphinx-apidoc.html
# add_custom_target(python_api
#     VERBATIM
#     COMMAND
#         ${CMAKE_COMMAND}
#             -E env "SPHINX_APIDOC_OPTIONS=members,undoc-members,show-inheritance,inherited-members"
#                 ${SPHINX_APIDOC_EXECUTABLE}
#                     --force
#                     --separate
#                     --module-first
#                     -d 1
#                     -o ${CMAKE_CURRENT_BINARY_DIR}/reference/python/api
#                     ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}
# )

configure_file(
    _config.yml.in
    _config.yml
    @ONLY
)

foreach(name IN ITEMS
    about/project.md
    about/license.md
    about/publication.md
    about/related.md
    about/release.md
    contribute/index.md
    contribute/documentation.md
    contribute/release.md
    example/index.md
    index.md
    install/conda.md
    install/index.md
    install/source.md
    manual/data_model/concept.md
    manual/data_model/format.md
    manual/data_model/index.md
    manual/framework/cpp/index.md
    manual/framework/execute.md
    manual/framework/index.md
    manual/framework/python/index.md
    manual/framework/scalability.md
    manual/view/index.md
    manual/glossary.md
    reference/algorithm/clump.md
    reference/algorithm/index.md
    # reference/cpp/dataset.md
    reference/cpp/index.md
    # reference/python/data_model/index.rst
    # reference/python/data_model/hdf5/index.rst
    # reference/python/framework/index.rst
    reference/python/index.md
    references.bib
    short_description.md
    pcraster/port.md
    pcraster/status.md
    reference/utility/index.md
    reference/utility/lue_calculate.md
    reference/utility/lue_scalability.md
    reference/utility/lue_translate.md
    reference/utility/lue_validate.md
    reference/utility/lue_view.md
    _toc.yml
)
    configure_file(${name} ${name} COPYONLY)
endforeach()


foreach(name IN ITEMS
    data_model/doc/index.md
    doc/index.md
    framework/algorithm/doc/design.md
    framework/algorithm/doc/index.md
    framework/algorithm/doc/policy/index.md
    framework/algorithm/doc/policy/no_data.md
    framework/algorithm/doc/policy/domain.md
    framework/algorithm/doc/policy/range.md
    framework/algorithm/doc/template.md

    framework/doc/howto/index.md
    framework/doc/howto/add_local_operation.md

    framework/algorithm/doc/group.dox

    framework/algorithm/doc/grouped_operation/index.md
    framework/algorithm/doc/grouped_operation/local_operation.md
    framework/algorithm/doc/grouped_operation/focal_operation.md
    framework/algorithm/doc/grouped_operation/zonal_operation.md
    framework/algorithm/doc/grouped_operation/global_operation.md
    framework/algorithm/doc/grouped_operation/routing_operation.md
    framework/algorithm/doc/grouped_operation/mathematical_operation.md
    framework/algorithm/doc/grouped_operation/random_number_operation.md

    framework/partitioned_array/doc/index.md
    framework/partitioned_route/doc/index.md

    framework/doc/index.md
)
    configure_file(${PROJECT_SOURCE_DIR}/source/${name} source/${name} COPYONLY)
endforeach()

add_custom_target(documentation
    VERBATIM
    COMMAND
        ${CMAKE_COMMAND}
            -E env "PYTHONPATH=${PROJECT_BINARY_DIR}/${LUE_INSTALL_PYTHON_PACKAGE_DIR}/..:$ENV{PYTHONPATH}"
                ${JupyterBook_EXECUTABLE} build --builder html
                    # --warningiserror --nitpick --keep-going
                    ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS
            doxygen_doc
            # python_api
)

set_property(
    DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY
        ADDITIONAL_CLEAN_FILES
            _build  # jupyter-book
            _config.yml
)
