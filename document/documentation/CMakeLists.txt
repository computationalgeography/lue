# Top-level target for generating files the JupyterBook documentation depends on
set(doc_preprocess_target_name lue.doc_preprocess)

add_custom_target(${doc_preprocess_target_name})

add_subdirectory(quick_start)
add_subdirectory(reference)

add_dependencies(${doc_preprocess_target_name}
    ${doc_preprocess_target_name}.reference
)

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
    genindex.md
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
    manual/install/apptainer.md
    manual/install/conda.md
    manual/install/easybuild.md
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
    modindex.md  # TODO: move to Python section of reference
    pcraster/index.md
    pcraster/port.md
    pcraster/status.md
    quick_start/cpp.md
    quick_start/game_of_life.cpp
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

    reference/concept/array.md
    reference/concept/field.md
    reference/concept/index.md
    reference/concept/raster.md
    reference/concept/scalar.md
    reference/concept/shape.md

    reference/operation/conversion/index.md

    reference/operation/focal/convolve.md
    reference/operation/focal/focal_diversity.md
    reference/operation/focal/focal_high_pass.md
    reference/operation/focal/index.md
    reference/operation/focal/focal_majority.md
    reference/operation/focal/focal_maximum.md
    reference/operation/focal/focal_mean.md
    reference/operation/focal/focal_minimum.md
    reference/operation/focal/focal_sum.md

    reference/operation/global/index.md

    reference/operation/index.md

    reference/operation/io/index.md

    reference/operation/local/index.md
    reference/operation/local/open_simplex_noise.md
    reference/operation/local/uniform.md

    reference/operation/miscellaneous/index.md
    reference/operation/miscellaneous/resample.md

    reference/operation/routing/index.md
    reference/operation/routing/d8_flow_direction.md
    reference/operation/routing/downstream.md
    reference/operation/routing/upstream.md

    reference/operation/zonal/index.md
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
    framework/partitioned_raster/doc/index.md
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
        ${CMAKE_COMMAND} -E env
            --modify PYTHONPATH=path_list_prepend:$<TARGET_FILE_DIR:lue::py>/..
            --modify LD_PRELOAD=path_list_append:$<$<AND:$<STREQUAL:${HPX_WITH_MALLOC},tcmalloc>,$<PLATFORM_ID:Linux>>:${Tcmalloc_LIBRARY}>
            --modify LD_PRELOAD=path_list_append:$<$<AND:$<STREQUAL:${HPX_WITH_MALLOC},jemalloc>,$<PLATFORM_ID:Linux>>:${Jemalloc_LIBRARY}>
            -- ${JupyterBook_EXECUTABLE} build --builder html --all
                # --warningiserror --nitpick --keep-going
                ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS
            doxygen_doc
            ${doc_preprocess_target_name}
            lue.figure.logo.colour
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
