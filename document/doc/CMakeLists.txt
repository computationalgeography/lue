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

configure_file(
    _config.yml.in
    _config.yml
    @ONLY
)

foreach(name IN ITEMS
    about/cite.md
    about/license.md
    about/project.md
    about/publication.md
    about/related.md
    about/release.md
    about/short_description.md
    contribute/developer_certificate_of_origin.txt
    contribute/documentation.md
    contribute/index.md
    contribute/release.md
    example/index.md
    faq.md
    index.md
    manual/data_model/concept.md
    manual/data_model/format.md
    manual/data_model/index.md
    manual/framework/cpp/index.md
    manual/framework/execute.md
    manual/framework/index.md
    manual/framework/python/index.md
    manual/glossary.md
    manual/index.md
    manual/install/conda.md
    manual/install/index.md
    manual/install/source.md
    manual/quality_assurance/index.md
    manual/quality_assurance/scalability/experiment.md
    manual/quality_assurance/scalability/game_of_life.py
    manual/quality_assurance/scalability/index.md
    manual/quality_assurance/scalability/partition_shape.svg
    manual/quality_assurance/scalability/quick_start.md
    manual/quality_assurance/scalability/scalability.sh
    manual/quality_assurance/scalability/strong_scalability.svg
    manual/quality_assurance/scalability/weak_scalability.svg
    manual/view/index.md
    pcraster/index.md
    pcraster/port.md
    pcraster/status.md
    quick_start/cpp.md
    quick_start/game_of_life.py
    quick_start/index.md
    quick_start/python.md
    reference/algorithm/clump.md
    reference/algorithm/flow_accumulation.md
    reference/algorithm/index.md
    # reference/cpp/dataset.md
    reference/c/index.md
    reference/cpp/index.md
    reference/index.md
    # reference/fortran/index.md
    # reference/java/index.md
    # reference/julia/index.md
    reference/operation/index.md
    reference/operation/resample.md
    reference/operation/uniform.md
    # reference/python/data_model/index.rst
    # reference/python/data_model/hdf5/index.rst
    # reference/python/data_model/index.rst
    # reference/python/framework/index.rst
    reference/python/index.md
    # reference/r/index.md
    # reference/rust/index.md
    references.bib
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

    framework/api/doc/index.md
    framework/api/cxx/doc/index.md

    framework/doc/test/index.md

    framework/doc/howto/index.md
    framework/doc/howto/add_local_operation.md
    framework/doc/howto/trace.md

    framework/algorithm/doc/group.dox

    framework/algorithm/doc/grouped_operation/index.md
    framework/algorithm/doc/grouped_operation/local_operation.md
    framework/algorithm/doc/grouped_operation/focal_operation.md
    framework/algorithm/doc/grouped_operation/zonal_operation.md
    framework/algorithm/doc/grouped_operation/global_operation.md
    framework/algorithm/doc/grouped_operation/routing_operation.md
    framework/algorithm/doc/grouped_operation/mathematical_operation.md
    framework/algorithm/doc/grouped_operation/array_operation.md
    framework/algorithm/doc/grouped_operation/random_number_operation.md

    framework/core/doc/index.md
    framework/partitioned_array/doc/index.md
    framework/partitioned_route/doc/index.md

    framework/doc/index.md

    quality_assurance/doc/index.md
    quality_assurance/cxx/doc/index.md
    quality_assurance/python/doc/index.md
)
    configure_file(${PROJECT_SOURCE_DIR}/source/${name} source/${name} COPYONLY)
endforeach()

add_custom_target(documentation
    VERBATIM
    COMMAND
        ${CMAKE_COMMAND}
            -E env "PYTHONPATH=${PROJECT_BINARY_DIR}/${LUE_INSTALL_PYTHON_PACKAGE_DIR}/..:$ENV{PYTHONPATH}"
                ${JupyterBook_EXECUTABLE} build --builder html --all
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
