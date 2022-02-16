set(scope lue_framework_core)
set(names
    array_test
    array_partition_data_test
    array_partition_definition_test
    array_visitor_test
    configuration_entry_test
    domain_decomposition_test
    hilbert_curve_test
    index_util_test
    linear_curve_test
    math_test
    shared_buffer_test
    shape_test
    span_test
)

foreach(name ${names})

    set(test_name ${scope}_${name})

    add_executable(${test_name} ${name}.cpp)

    target_include_directories(${test_name} SYSTEM
        PRIVATE
            ${Boost_INCLUDE_DIRS}
    )

    target_link_libraries(${test_name}
        PRIVATE
            lue::framework_core
            lue::framework_test
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND ${test_name}
    )

    set_tests_properties(
        ${test_name}
        PROPERTIES
            # On Windows, add the path to the HPX dlls. Only if HPX::hpx target is defined.
            ENVIRONMENT_MODIFICATION
                PATH=path_list_prepend:$<$<AND:$<PLATFORM_ID:Windows>,$<TARGET_EXISTS:HPX::hpx>>:$<TARGET_FILE_DIR:HPX::component_storage_component>>
    )
endforeach()
