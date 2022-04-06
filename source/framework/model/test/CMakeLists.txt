set(scope lue_framework_model)
set(names
    model
    simulate
)

foreach(name ${names})

    set(module_name ${name}_test)
    set(test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_link_libraries(${test_name}
        PRIVATE
            lue::framework_model
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
