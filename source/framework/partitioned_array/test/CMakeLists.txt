set(scope lue_framework_partitioned_array)
set(names
    array_partition_data
    shared_buffer
)

foreach(name ${names})

    set(module_name ${name}_test)
    set(test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_link_libraries(${test_name}
        PRIVATE
            lue::framework_core
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

set(names
    array_partition
    partitioned_array
)

foreach(name ${names})

    set(module_name ${name}_test)
    set(test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_link_libraries(${test_name}
        PRIVATE
            lue::framework_partitioned_array
            lue::framework_test
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND
            ${Python3_EXECUTABLE} ${HPXRUN}
                "--runwrapper" ${LUE_QA_TEST_HPX_RUNWRAPPER}
                "--parcelport" ${LUE_QA_TEST_HPX_PARCELPORT}
                "--localities" ${LUE_QA_TEST_NR_LOCALITIES_PER_TEST}
                "--thread" ${LUE_QA_TEST_NR_THREADS_PER_LOCALITY} --
                    "$<TARGET_FILE:${test_name}>"
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
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_partitioned_array>
        )
    endif()
endforeach()
