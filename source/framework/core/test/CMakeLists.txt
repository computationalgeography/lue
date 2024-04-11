set(scope lue_framework_core)
set(names
    array
    array_visitor
    configuration_entry
    domain_decomposition
    hilbert_curve
    index_util
    linear_curve
    math
    shape
    span
)

foreach(name ${names})
    set(module_name ${name}_test)
    string(REPLACE "/" "_" test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_compile_definitions(${test_name}
        PRIVATE
            BOOST_TEST_DYN_LINK
    )

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
