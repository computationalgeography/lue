set(scope lue_shared)
set(names
    no_data
)

foreach(name ${names})
    set(module_name ${name}_test)
    string(REPLACE "/" "_" test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_link_libraries(${test_name}
        PRIVATE
            lue::shared
            Boost::disable_autolinking
            Boost::unit_test_framework
            Boost::dynamic_linking
    )
    add_test(NAME ${test_name}
        COMMAND ${test_name}
    )
    set_property(
        TEST
            ${test_name}
        APPEND
        PROPERTY
            ENVIRONMENT_MODIFICATION
                PATH=path_list_prepend:${Boost_LIBRARY_DIRS}
    )
endforeach()
