set(scope lue_framework_core)
set(names
    array_test
    array_visitor_test
    configuration_entry_test
    domain_decomposition_test
    hilbert_curve_test
    index_util_test
    linear_curve_test
    math_test
    shape_test
    span_test
)

foreach(name ${names})

    set(test_name ${scope}_${name})

    add_executable(${test_name} ${name}.cpp)

    target_link_libraries(${test_name}
        PRIVATE
            lue::framework_test
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND ${test_name}
    )

    if(WIN32)
        # Ensure required DLLs can be found at runtime
        set_property(
            TEST
                ${test_name}
            APPEND
            PROPERTY
                ENVIRONMENT_MODIFICATION
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:HPX::component_storage_component>
        )
    endif()
endforeach()
