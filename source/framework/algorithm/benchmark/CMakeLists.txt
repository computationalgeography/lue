function(add_benchmark)
    set(OPTIONS "")
    set(ONE_VALUE_ARGUMENTS NAME)
    set(MULTI_VALUE_ARGUMENTS "")

    cmake_parse_arguments(ADD_BENCHMARK "${OPTIONS}"
        "${ONE_VALUE_ARGUMENTS}" "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(ADD_BENCHMARK_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${ADD_BENCHMARK_UNPARSED_ARGUMENTS}"
        )
    endif()

    set(name ${ADD_BENCHMARK_NAME})

    add_hpx_executable(lue_framework_algorithm_${name}_benchmark
        SOURCES
            ${name}_benchmark
        COMPONENT_DEPENDENCIES
            iostreams
    )

    target_link_libraries(lue_framework_algorithm_${name}_benchmark
        PRIVATE
            lue::framework_algorithm
            lue::benchmark
    )
endfunction()


add_benchmark(NAME copy)
add_benchmark(NAME empty)
add_benchmark(NAME focal_mean)
add_benchmark(NAME iterate_per_element)
add_benchmark(NAME sqrt)
