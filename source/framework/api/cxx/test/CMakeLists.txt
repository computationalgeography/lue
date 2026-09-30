set(scope lue_framework_api_cxx)

set(focal_operation_names
    focal/aspect
    focal/convolve
    focal/diversity
    focal/high_pass
    focal/majority
    focal/maximum
    focal/mean
    focal/minimum
    focal/slope
    focal/sum
)

set(local_operation_names
    local/abs
    local/acos
    local/add
    local/asin
    local/atan
    local/atan2
    local/ceil
    local/close_to
    local/cos
    local/divide
    local/equal_to
    local/exp
    local/floor
    local/greater_than
    local/greater_than_equal_to
    local/less_than
    local/less_than_equal_to
    local/log
    local/log10
    local/logical_and
    local/logical_exclusive_or
    local/logical_inclusive_or
    local/logical_not
    local/modulus
    local/multiply
    local/negate
    local/not_equal_to
    local/open_simplex_noise
    local/pow
    local/round
    local/sin
    local/sqrt
    local/subtract
    local/tan
    local/trunc
    local/uniform
    local/valid
    local/where
)

set(miscellaneous_operation_names
    miscellaneous/array_like
    miscellaneous/cast
    miscellaneous/cell_index
    miscellaneous/create_array
    miscellaneous/create_scalar
)

set(routing_operation_names
    routing/accu
    routing/d8_flow_direction
)

set(names
    ${focal_operation_names}
    # io
    ${local_operation_names}
    ${miscellaneous_operation_names}
    ${routing_operation_names}
    field
)

foreach(name ${names})
    set(module_name ${name}_test)
    string(REPLACE "/" "_" test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_include_directories(${test_name}
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

    target_link_libraries(${test_name}
        PRIVATE
            lue::framework_api_cxx
            Boost::headers
    )

    add_test(NAME ${test_name}
        COMMAND
            ${Python_EXECUTABLE} ${HPXRUN}
                "--runwrapper" ${LUE_QUALITY_ASSURANCE_TEST_HPX_RUNWRAPPER}
                "--parcelport" ${LUE_QUALITY_ASSURANCE_TEST_HPX_PARCELPORT}
                "--localities" ${LUE_QUALITY_ASSURANCE_TEST_NR_LOCALITIES_PER_TEST}
                "--thread" ${LUE_QUALITY_ASSURANCE_TEST_NR_THREADS_PER_LOCALITY} --
                    "$<TARGET_FILE:${test_name}>"
    )

    set_property(
        TEST
            ${test_name}
        APPEND
        PROPERTY
            ENVIRONMENT_MODIFICATION
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_api_cxx>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_io>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_local_operation>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_focal_operation>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_routing_operation>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_zonal_operation>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_partitioned_array>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_partitioned_route>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::data_model>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::gdal>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::hdf5>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:HPX::component_storage_component>>
    )

    append_3rd_party_runtime_lib_dirs(${test_name})
endforeach()

# foreach(name ${local_operation_names})
#     set(module_name ${name}_test)
#     string(REPLACE "/" "_" test_name ${module_name})
#     set(test_name ${scope}_${test_name})
#
#     target_link_libraries(${test_name}
#         PRIVATE
#             lue_framework_api_cxx
#     )
# endforeach()


set(scope lue.framework.api.cxx.test.stress)
set(names
    # game_of_life
    local_operation
)

foreach(name ${names})
    set(target_name ${scope}.${name})
    add_executable(${target_name} stress/${name}.cpp)

    target_link_libraries(${target_name}
        PRIVATE
            lue::framework_api_cxx
            lue::framework_model
            lue::utility
            HPX::iostreams_component
            HPX::wrap_main
    )
endforeach()
