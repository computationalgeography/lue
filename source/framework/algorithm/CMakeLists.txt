add_test_conditionally(test)

# if(LUE_FRAMEWORK_WITH_BENCHMARKS)
#     add_subdirectory(benchmark)
# endif()

add_library(lue_framework_algorithm SHARED
    src/component/array_partition_io.cpp
)

add_library(lue::framework_algorithm ALIAS lue_framework_algorithm)

set_target_properties(lue_framework_algorithm
    PROPERTIES
        EXPORT_NAME framework_algorithm
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

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

target_sources(lue_framework_algorithm
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/export.hpp
)

target_link_libraries(lue_framework_algorithm
    PUBLIC
        lue::framework_partitioned_array
        lue::framework_core
        # HPX::iostreams_component
        HPX::hpx
)


# TODO Maybe set these at a higher level?
set(LUE_FRAMEWORK_POLICIES "DefaultPolicies" "DefaultValuePolicies")
set(ranks 2)


# Instantiate local operations -------------------------------------------------
set(generated_source_files "")
set(offset "src/local_operation")

block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
            foreach(rank IN LISTS ranks)
                math(EXPR count "${count} + 1")

                # Instantiate add
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/add-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/add.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate equal_to
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/equal_to-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/equal_to.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate greater_than
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/greater_than-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/greater_than.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate greater_than_equal_to
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/greater_than_equal_to-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/greater_than_equal_to.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate less_than
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/less_than-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/less_than.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate less_than_equal_to
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/less_than_equal_to-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/less_than_equal_to.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate not_equal_to
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/not_equal_to-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/not_equal_to.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate valid
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/valid-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/valid.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate multiply
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/multiply-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/multiply.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate subtract
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/subtract-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/subtract.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate where2
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/where2-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/where2.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","ConditionElement":"${LUE_FRAMEWORK_CONDITION_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate where3
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/where3-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/where3.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","ConditionElement":"${LUE_FRAMEWORK_CONDITION_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS
                LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS
                LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS
            )
            foreach(rank IN LISTS ranks)
                math(EXPR count "${count} + 1")

                # Instantiate abs
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/abs-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/abs.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS
                LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS
            )
            foreach(rank IN LISTS ranks)
                math(EXPR count "${count} + 1")

                # acos
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/acos-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/acos.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # asin
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/asin-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/asin.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # atan
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/atan-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/atan.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # atan2
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/atan2-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/atan2.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate close_to
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/close_to-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/close_to.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # cos
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/cos-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/cos.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # divide
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/divide-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/divide.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # exp
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/exp-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/exp.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # log
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/log-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/log.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # pow
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/pow-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/pow.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # sin
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/sin-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/sin.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # sqrt
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/sqrt-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/sqrt.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # tan
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/tan-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/tan.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(OutputElement IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            foreach(InputElement IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
                foreach(rank IN LISTS ranks)
                    math(EXPR count "${count} + 1")

                    # Instantiate cast
                    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/cast-${count}.cpp")

                    generate_template_instantiation(
                        INPUT_PATHNAME
                            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/cast.cpp.in"
                        OUTPUT_PATHNAME
                            "${output_pathname}"
                        DICTIONARY
                            '{"Policies":"${Policies}","OutputElement":"${OutputElement}","InputElement":"${InputElement}","rank":"${rank}"}'
                    )
                    list(APPEND generated_source_files "${output_pathname}")
                endforeach()
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            foreach(rank IN LISTS ranks)
                math(EXPR count "${count} + 1")

                # Instantiate logical_and
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/logical_and-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/logical_and.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate logical_exclusive_or
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/logical_exclusive_or-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/logical_exclusive_or.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate logical_inclusive_or
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/logical_inclusive_or-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/logical_inclusive_or.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate logical_not
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/logical_not-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/logical_not.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate unique_id
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/unique_id-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/unique_id.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","ConditionElement":"${LUE_FRAMEWORK_CONDITION_ELEMENT}","IDElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()
# / Instantiate local operations -----------------------------------------------

add_library(lue_framework_local_operation SHARED
    ${generated_source_files}
)

add_library(lue::framework_local_operation ALIAS lue_framework_local_operation)

set_target_properties(lue_framework_local_operation
    PROPERTIES
        EXPORT_NAME framework_local_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_local_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/local_operation_export.hpp
    EXPORT_MACRO_NAME LUE_LOCAL_OPERATION_EXPORT)

target_include_directories(lue_framework_local_operation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_sources(lue_framework_local_operation
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/local_operation_export.hpp
)

target_link_libraries(lue_framework_local_operation
    PUBLIC
        lue::framework_algorithm
)


# Instantiate focal operations -------------------------------------------------
set(generated_source_files "")
set(offset "src/focal_operation")
set(LUE_FRAMEWORK_FOCAL_OPERATION_POLICIES "DefaultValuePolicies")
set(Counts std::uint64_t)


block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_FOCAL_OPERATION_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
            math(EXPR count "${count} + 1")

            # Instantiate focal_maximum
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_maximum-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/focal_maximum.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Boolean":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate focal_minimum
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_minimum-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/focal_minimum.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Boolean":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate focal_sum
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_sum-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/focal_sum.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Boolean":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()


block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_FOCAL_OPERATION_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            math(EXPR count "${count} + 1")

            # Instantiate aspect
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/aspect-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/aspect.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate focal_high_pass
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_high_pass-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/focal_high_pass.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Boolean":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate focal_mean
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_mean-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/focal_mean.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Boolean":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate gradients
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/gradients-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/gradients.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate slope
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/slope-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/slope.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()


block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_FOCAL_OPERATION_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            math(EXPR count "${count} + 1")

            # Instantiate focal_majority
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_majority-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/focal_majority.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Boolean":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()


block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_FOCAL_OPERATION_POLICIES)
        foreach(Count IN LISTS Counts)
            foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
                math(EXPR count "${count} + 1")

                # Instantiate focal_diversity
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_diversity-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/focal_diversity.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","Count":"${Count}","Element":"${Element}","Boolean":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()


add_library(lue_framework_focal_operation SHARED
    ${generated_source_files}
)

add_library(lue::framework_focal_operation ALIAS lue_framework_focal_operation)

set_target_properties(lue_framework_focal_operation
    PROPERTIES
        EXPORT_NAME framework_focal_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_focal_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/focal_operation_export.hpp
    EXPORT_MACRO_NAME LUE_FOCAL_OPERATION_EXPORT)

target_include_directories(lue_framework_focal_operation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_sources(lue_framework_focal_operation
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/focal_operation_export.hpp
)

target_link_libraries(lue_framework_focal_operation
    PUBLIC
        lue::framework_algorithm
)


# TODO Maybe set these at a higher level?
set(LUE_FRAMEWORK_POLICIES "DefaultValuePolicies")
set(Counts std::uint64_t)
set(Zones std::uint8_t std::uint32_t std::uint64_t std::int32_t std::int64_t)


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
            foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
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
        foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
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
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
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
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
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
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
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
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
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
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
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
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
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

set_target_properties(lue_framework_zonal_operation
    PROPERTIES
        EXPORT_NAME framework_zonal_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_zonal_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/zonal_operation_export.hpp
    EXPORT_MACRO_NAME LUE_ZONAL_OPERATION_EXPORT)

target_include_directories(lue_framework_zonal_operation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_sources(lue_framework_zonal_operation
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/zonal_operation_export.hpp
)

target_link_libraries(lue_framework_zonal_operation
    PUBLIC
        lue::framework_algorithm
        lue::framework_global_operation  # unique
)



# Instantiate global operations ------------------------------------------------
set(all_policies "DefaultPolicies" "DefaultValuePolicies")
set(generated_source_files "")
set(offset "src/global_operation")


block()
    set(counter "0")

    foreach(Policies IN LISTS all_policies)
        foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            foreach(rank IN LISTS ranks)
                math(EXPR count "${count} + 1")

                # Instantiate unique
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/unique-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/unique.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(InputElement IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            foreach(OutputElement IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
                foreach(rank IN LISTS ranks)
                    math(EXPR count "${count} + 1")

                    # Instantiate reclassify
                    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/reclassify-${count}.cpp")

                    generate_template_instantiation(
                        INPUT_PATHNAME
                            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/reclassify.cpp.in"
                        OUTPUT_PATHNAME
                            "${output_pathname}"
                        DICTIONARY
                            '{"Policies":"${Policies}","InputElement":"${InputElement}","OutputElement":"${OutputElement}","rank":"${rank}"}'
                    )
                    list(APPEND generated_source_files "${output_pathname}")
                endforeach()
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()
# / Instantiate global operations ----------------------------------------------


add_library(lue_framework_global_operation SHARED
    ${generated_source_files}
)

add_library(lue::framework_global_operation ALIAS lue_framework_global_operation)

set_target_properties(lue_framework_global_operation
    PROPERTIES
        EXPORT_NAME framework_global_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_global_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/global_operation_export.hpp
    EXPORT_MACRO_NAME LUE_GLOBAL_OPERATION_EXPORT)

target_include_directories(lue_framework_global_operation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_sources(lue_framework_global_operation
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/global_operation_export.hpp
)

target_link_libraries(lue_framework_global_operation
    PUBLIC
        lue::framework_algorithm
)


# Instantiate routing operations -----------------------------------------------
set(all_policies "DefaultPolicies" "DefaultValuePolicies")
set(generated_source_files "")
set(offset "src/routing_operation")


block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        math(EXPR count "${count} + 1")

        # Instantiate accu_info
        set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu_info-${count}.cpp")

        generate_template_instantiation(
            INPUT_PATHNAME
                "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/accu_info.cpp.in"
            OUTPUT_PATHNAME
                "${output_pathname}"
            DICTIONARY
                '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}"}'
        )
        list(APPEND generated_source_files "${output_pathname}")

        # Instantiate accu_info3
        string(REPLACE "::" "_" name ${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT})
        set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu_info3-${count}.cpp")

        generate_template_instantiation(
            INPUT_PATHNAME
                "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/accu_info3.cpp.in"
            OUTPUT_PATHNAME
                "${output_pathname}"
            DICTIONARY
                '{"name:":"${name}","Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}"}'
        )
        list(APPEND generated_source_files "${output_pathname}")
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

set(material_elements std::uint64_t ${LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS})

block()
    set(counter "0")

    foreach(Policies IN LISTS all_policies)
        foreach(Element IN LISTS material_elements)
            math(EXPR count "${count} + 1")

            # Instantiate accu
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/accu.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS material_elements)
            math(EXPR count "${count} + 1")

            # Instantiate upstream
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/upstream-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/upstream.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    set(counter "0")

    foreach(Policies IN LISTS all_policies)
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            math(EXPR count "${count} + 1")
            string(REPLACE "::" "_" name ${Element})

            # Instantiate accu3
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu3-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/accu3.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"name":"${name}","Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()


block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            math(EXPR count "${count} + 1")
            string(REPLACE "::" "_" name ${Element})

            # Instantiate accu_fraction
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu_fraction-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/accu_fraction.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate accu_threshold
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu_threshold-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/accu_threshold.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate accu_threshold3
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu_threshold3-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/accu_threshold3.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"name":"${name}","Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate d8_flow_direction
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/d8_flow_direction-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/d8_flow_direction.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate kinematic_wave
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/kinematic_wave-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/kinematic_wave.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"name":"${name}","Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

set(materials std::uint8_t std::int64_t ${LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS})

block()
    set(counter "0")

    foreach(Element IN LISTS materials)
        math(EXPR count "${count} + 1")
        string(REPLACE "::" "_" name ${Element})

        # Instantiate channel_material
        set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/channel_material-${count}.cpp")

        generate_template_instantiation(
            INPUT_PATHNAME
                "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/channel_material.cpp.in"
            OUTPUT_PATHNAME
                "${output_pathname}"
            DICTIONARY
                '{"name":"${name}","Element":"${Element}"}'
        )
        list(APPEND generated_source_files "${output_pathname}")
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
            math(EXPR count "${count} + 1")

            # Instantiate downstream
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/downstream-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/downstream.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            math(EXPR count "${count} + 1")

            # Instantiate downstream_distance
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/downstream_distance-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/downstream_distance.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    set(counter "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_POLICIES)
        math(EXPR count "${count} + 1")

        # Instantiate inflow_count3
        set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/inflow_count3-${count}.cpp")

        generate_template_instantiation(
            INPUT_PATHNAME
                "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/inflow_count3.cpp.in"
            OUTPUT_PATHNAME
                "${output_pathname}"
            DICTIONARY
            '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"std::uint8_t"}'
        )
        list(APPEND generated_source_files "${output_pathname}")
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

# / Instantiate routing operations ---------------------------------------------


add_library(lue_framework_routing_operation SHARED
    ${generated_source_files}
    src/routing_operation/channel.cpp
)

add_library(lue::framework_routing_operation ALIAS lue_framework_routing_operation)

set_target_properties(lue_framework_routing_operation
    PROPERTIES
        EXPORT_NAME framework_routing_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_routing_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/routing_operation_export.hpp
    EXPORT_MACRO_NAME LUE_ROUTING_OPERATION_EXPORT)

target_include_directories(lue_framework_routing_operation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_sources(lue_framework_routing_operation
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/routing_operation_export.hpp
)

target_link_libraries(lue_framework_routing_operation
    PUBLIC
        lue::framework_algorithm
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_algorithm
        lue_framework_local_operation
        lue_framework_routing_operation
        lue_framework_focal_operation
        lue_framework_global_operation
        lue_framework_zonal_operation
)

lue_install_development_libraries(
    TARGETS
        lue_framework_algorithm
        lue_framework_local_operation
        lue_framework_routing_operation
        lue_framework_focal_operation
        lue_framework_global_operation
        lue_framework_zonal_operation
)
