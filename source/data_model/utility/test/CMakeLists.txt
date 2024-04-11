set(scope lue_utility)
set(names
    environment
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
            lue::utility
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND ${test_name}
    )
endforeach()
