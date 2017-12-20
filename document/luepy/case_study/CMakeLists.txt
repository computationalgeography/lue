add_custom_target(
    case_study
    DEPENDS case_study_graphs
)

add_custom_target(
    case_study_graphs
    DEPENDS preprocessed_case_studies
)

add_custom_target(
    preprocessed_case_studies
)

function(preprocess_case_study)
    set(one_value_args SCRIPT)

    cmake_parse_arguments(DATASET
        "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    set(script_name ${DATASET_SCRIPT})
    get_filename_component(basename ${script_name} NAME_WE)
    set(dataset_name ${basename}.lue)
    set(dot_name ${basename}.dot)
    set(svg_name ${basename}.svg)
    set(target_name ${basename}_target)

    set(dot_properties_pathname
        ${CMAKE_SOURCE_DIR}/document/lue_translate/dot_properties.json)

    add_custom_command(
        OUTPUT ${svg_name}
        DEPENDS
            ${script_name}
            lue::py
            lue_validate
            lue_translate
            ${dot_properties_pathname}
        COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/${script_name}
        COMMAND lue_validate --issues ${dataset_name}
        COMMAND lue_translate export
            --meta ${dot_properties_pathname} ${dataset_name} ${dot_name}
        COMMAND ${GRAPHVIZ_DOT_EXECUTABLE} -Tsvg -o ${svg_name} ${dot_name}
    )

    add_custom_target(${target_name}
        DEPENDS ${svg_name}
    )

    add_dependencies(preprocessed_case_studies
        ${target_name})
endfunction()


function(preprocess_case_studies)
    set(multi_value_args SCRIPTS)

    cmake_parse_arguments(DATASET
        "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    foreach(script_name ${DATASET_SCRIPTS})
        preprocess_case_study(SCRIPT ${script_name})
    endforeach()
endfunction()


preprocess_case_studies(
    SCRIPTS
        areas.py
        cities.py
        elevation.py
        planets.py
)
