set(scope lue_framework_api_c)

set(names
    io
    local_operation
)

foreach(name ${names})
    set(module_name ${name}_test)
    string(REPLACE "/" "_" test_name ${module_name})
    set(test_name ${scope}_${test_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_link_libraries(${test_name}
        PUBLIC
            lue::framework_api_c
        PRIVATE
            lue::framework_test
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND
            ${Python_EXECUTABLE} ${HPXRUN}
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
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_api_cxx>
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_partitioned_array>
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_partitioned_route>
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_local_operation>
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_focal_operation>
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_routing_operation>
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_zonal_operation>
        )
    endif()
endforeach()
