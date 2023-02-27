add_test_conditionally(test)

# if(LUE_FRAMEWORK_WITH_BENCHMARKS)
#     add_subdirectory(benchmark)
# endif()

add_library(lue_framework_algorithm SHARED
    src/component/array_partition_io.cpp
)

add_library(lue::framework_algorithm ALIAS lue_framework_algorithm)

generate_export_header(lue_framework_algorithm
    EXPORT_FILE_NAME include/lue/framework/algorithm/export.hpp
    EXPORT_MACRO_NAME LUE_FA_EXPORT)

# set_target_properties(lue_framework_algorithm
#     PROPERTIES
#         CXX_CLANG_TIDY ""  # TODO(KDJ) Get it to skip hpx headers
# )

target_include_directories(lue_framework_algorithm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_link_libraries(lue_framework_algorithm
    PUBLIC
        lue::framework_partitioned_array
        lue::framework_core
        # HPX::iostreams_component
        HPX::hpx
)


# Libraries containing explicit instantiations of modelling operations
# supporting map algebra:
# - 2D arrays
# - Common element types
# - Common policies
add_library(lue_framework_local_operation SHARED
    src/local_operation/abs.cpp
    src/local_operation/add.cpp
    src/local_operation/acos.cpp
    src/local_operation/asin.cpp
    src/local_operation/atan.cpp
    src/local_operation/atan2.cpp
    src/local_operation/cast.cpp
    src/local_operation/close_to.cpp
    src/local_operation/cos.cpp
    src/local_operation/divide.cpp
    src/local_operation/equal_to.cpp
    src/local_operation/exp.cpp
    src/local_operation/greater_than.cpp
    src/local_operation/greater_than_equal_to.cpp
    src/local_operation/less_than.cpp
    src/local_operation/less_than_equal_to.cpp
    src/local_operation/log.cpp
    src/local_operation/logical_and.cpp
    src/local_operation/logical_exclusive_or.cpp
    src/local_operation/logical_inclusive_or.cpp
    src/local_operation/logical_not.cpp
    src/local_operation/multiply.cpp
    src/local_operation/not_equal_to.cpp
    src/local_operation/pow.cpp
    src/local_operation/sin.cpp
    src/local_operation/sqrt.cpp
    src/local_operation/subtract.cpp
    src/local_operation/tan.cpp
    src/local_operation/unique_id.cpp
    src/local_operation/valid.cpp
    src/local_operation/default_policies/where.cpp
    src/local_operation/value_policies/where.cpp
)

add_library(lue::framework_local_operation ALIAS lue_framework_local_operation)

generate_export_header(lue_framework_local_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/local_operation_export.hpp
    EXPORT_MACRO_NAME LUE_LOCAL_OPERATION_EXPORT)

target_include_directories(lue_framework_local_operation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_link_libraries(lue_framework_local_operation
    PUBLIC
        lue::framework_algorithm
)


add_library(lue_framework_focal_operation SHARED
    src/focal_operation/aspect.cpp
    src/focal_operation/convolve.cpp
    src/focal_operation/focal_diversity.cpp
    src/focal_operation/focal_high_pass.cpp
    src/focal_operation/focal_majority.cpp
    src/focal_operation/focal_maximum.cpp
    src/focal_operation/focal_mean.cpp
    src/focal_operation/focal_minimum.cpp
    src/focal_operation/focal_sum.cpp
    src/focal_operation/gradients.cpp
    src/focal_operation/slope.cpp
)

add_library(lue::framework_focal_operation ALIAS lue_framework_focal_operation)

generate_export_header(lue_framework_focal_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/focal_operation_export.hpp
    EXPORT_MACRO_NAME LUE_FOCAL_OPERATION_EXPORT)

target_include_directories(lue_framework_focal_operation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_link_libraries(lue_framework_focal_operation
    PUBLIC
        lue::framework_algorithm
)


# TODO A global project configuration setting. Move to LueConfiguration.cmake
list(APPEND LUE_FRAMEWORK_POLICIES "DefaultValuePolicies")

# TODO A setting specific for zonal operation. Maybe also global?
set(Counts std::uint64_t)

# TODO A setting specific for zonal operation. Maybe also global?
set(Zones std::uint8_t std::uint32_t std::uint64_t std::int32_t std::int64_t)

# TODO A setting specific for zonal operation. Maybe also global?
set(IntegralElements std::uint8_t std::uint32_t std::uint64_t std::int32_t std::int64_t)

# TODO A setting specific for zonal operation. Maybe also global?
set(FloatingPointElements float double)

# TODO A setting specific for zonal operation. Maybe also global?
set(Elements ${IntegralElements} ${FloatingPointElements})

# TODO Global variable. Move to LueConfiguration or so.
set(LUE_TEMPLATIZE "${PROJECT_SOURCE_DIR}/environment/script/templatize.py")


# Instantiate zonal operations -------------------------------------------------
set(generated_source_files "")

block()
    # Instantiate zonal_area
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Count IN LISTS Counts)
            foreach(Zone IN LISTS Zones)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/zonal_operation/zonal_area-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/src/zonal_operation/zonal_area.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","Count":"${Count}","Zone":"${Zone}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_diversity
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Count IN LISTS Counts)
            foreach(Element IN LISTS IntegralElements)
                foreach(Zone IN LISTS Zones)
                    math(EXPR count "${count} + 1")

                    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/zonal_operation/zonal_diversity-${count}.cpp")

                    generate_template_instantiation(
                        INPUT_PATHNAME
                            "${CMAKE_CURRENT_SOURCE_DIR}/src/zonal_operation/zonal_diversity.cpp.in"
                        OUTPUT_PATHNAME
                            "${output_pathname}"
                        DICTIONARY
                        '{"Policies":"${Policies}","Count":"${Count}","Element":"${Element}","Zone":"${Zone}"}'
                    )
                    list(APPEND generated_source_files "${output_pathname}")
                endforeach()
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_majority
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS IntegralElements)
            foreach(Zone IN LISTS Zones)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/zonal_operation/zonal_majority-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/src/zonal_operation/zonal_majority.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Zone":"${Zone}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_maximum
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS Elements)
            foreach(Zone IN LISTS Zones)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/zonal_operation/zonal_maximum-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/src/zonal_operation/zonal_maximum.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Zone":"${Zone}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_mean
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS FloatingPointElements)
            foreach(Zone IN LISTS Zones)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/zonal_operation/zonal_mean-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/src/zonal_operation/zonal_mean.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Zone":"${Zone}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_minimum
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS Elements)
            foreach(Zone IN LISTS Zones)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/zonal_operation/zonal_minimum-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/src/zonal_operation/zonal_minimum.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Zone":"${Zone}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_normal
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS FloatingPointElements)
            foreach(Zone IN LISTS Zones)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/zonal_operation/zonal_normal-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/src/zonal_operation/zonal_normal.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Zone":"${Zone}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_sum
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS Elements)
            foreach(Zone IN LISTS Zones)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/zonal_operation/zonal_sum-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/src/zonal_operation/zonal_sum.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Zone":"${Zone}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_uniform
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS FloatingPointElements)
            foreach(Zone IN LISTS Zones)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/zonal_operation/zonal_uniform-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/src/zonal_operation/zonal_uniform.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Zone":"${Zone}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()
# /Instantiate zonal operations ------------------------------------------------

add_library(lue_framework_zonal_operation SHARED
    ${generated_source_files}
)

add_library(lue::framework_zonal_operation ALIAS lue_framework_zonal_operation)

generate_export_header(lue_framework_zonal_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/zonal_operation_export.hpp
    EXPORT_MACRO_NAME LUE_ZONAL_OPERATION_EXPORT)

target_include_directories(lue_framework_zonal_operation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_link_libraries(lue_framework_zonal_operation
    PUBLIC
        lue::framework_algorithm
        lue::framework_global_operation  # unique
)


add_library(lue_framework_global_operation SHARED
    src/global_operation/reclassify.cpp
    src/global_operation/unique.cpp
)

add_library(lue::framework_global_operation ALIAS lue_framework_global_operation)

generate_export_header(lue_framework_global_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/global_operation_export.hpp
    EXPORT_MACRO_NAME LUE_GLOBAL_OPERATION_EXPORT)

target_include_directories(lue_framework_global_operation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_link_libraries(lue_framework_global_operation
    PUBLIC
        lue::framework_algorithm
)

add_library(lue_framework_routing_operation SHARED
    src/routing_operation/accu.cpp
    src/routing_operation/accu3.cpp
    src/routing_operation/accu_fraction.cpp
    src/routing_operation/accu_info.cpp
    src/routing_operation/accu_info3.cpp
    src/routing_operation/accu_threshold.cpp
    src/routing_operation/accu_threshold3.cpp
    src/routing_operation/d8_flow_direction.cpp
    src/routing_operation/downstream.cpp
    src/routing_operation/downstream_distance.cpp
    src/routing_operation/flow_accumulation3.cpp
    src/routing_operation/inflow_count3.cpp
    src/routing_operation/kinematic_wave.cpp
    src/routing_operation/upstream.cpp
)

add_library(lue::framework_routing_operation ALIAS lue_framework_routing_operation)

generate_export_header(lue_framework_routing_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/routing_operation_export.hpp
    EXPORT_MACRO_NAME LUE_ROUTING_OPERATION_EXPORT)

target_include_directories(lue_framework_routing_operation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_link_libraries(lue_framework_routing_operation
    PUBLIC
        lue::framework_algorithm
)

lue_install_libraries(
    TARGETS
        lue_framework_algorithm
        lue_framework_local_operation
        lue_framework_routing_operation
        lue_framework_focal_operation
        lue_framework_global_operation
        lue_framework_zonal_operation
)
