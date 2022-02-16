set(scope lue_framework_io)
set(names
    write
)

foreach(name ${names})

    set(module_name ${name}_test)
    set(test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_include_directories(${test_name} SYSTEM
        PRIVATE
            ${Boost_INCLUDE_DIRS}
    )

    target_link_libraries(${test_name}
        PRIVATE
            lue::framework_io
            lue::framework_test
            lue::data_model_hl
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND
            ${Python3_EXECUTABLE} ${HPXRUN}
                "--runwrapper" ${LUE_TEST_HPX_RUNWRAPPER}
                "--parcelport" ${LUE_TEST_HPX_PARCELPORT}
                "--localities" ${LUE_TEST_NR_LOCALITIES_PER_TEST}
                "--thread" ${LUE_TEST_NR_THREADS_PER_LOCALITY} --
                    "$<TARGET_FILE:${test_name}>"
    )

    set_tests_properties(
        ${test_name}
        PROPERTIES
            # On Windows, add the path to the HPX dlls. Only if HPX::hpx target is defined.
            ENVIRONMENT_MODIFICATION
                PATH=path_list_prepend:$<$<AND:$<PLATFORM_ID:Windows>,$<TARGET_EXISTS:HPX::hpx>>:$<TARGET_FILE_DIR:HPX::component_storage_component>>
    )
endforeach()
