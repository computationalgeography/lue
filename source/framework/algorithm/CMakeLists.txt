### # Instantiate components -------------------------------------------------------
### set(generated_source_files "")
###
### block()
###     set(offset "include/lue/framework/algorithm/component/server")
###     set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/array_partition_io.hpp")
###
###     set(Elements ${LUE_FRAMEWORK_MATERIAL_ELEMENTS})
###     list(TRANSFORM Elements PREPEND "\"")
###     list(TRANSFORM Elements APPEND "\"")
###     list(JOIN Elements ", " Elements)
###
###     generate_template_instantiation(
###         INPUT_PATHNAME
###             "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/array_partition_io.hpp.in"
###         OUTPUT_PATHNAME
###             "${output_pathname}"
###         DICTIONARY
###             '{"Elements":[${Elements}],"ranks":[${LUE_FRAMEWORK_RANKS}]}'
###     )
###
###     list(APPEND generated_source_files "${output_pathname}")
###
###     set(generated_source_files ${generated_source_files} PARENT_SCOPE)
### endblock()
###
### block()
###     set(offset "source/component")
###
###     foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
###         foreach(Element IN LISTS LUE_FRAMEWORK_MATERIAL_ELEMENTS)
###             foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)
###                 string(REPLACE "::" "_" element ${Element})
###
###                 # Instantiate array_partition_io
###                 set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/array_partition_io-${Policies}_${element}.cpp")
###
###                 generate_template_instantiation(
###                     INPUT_PATHNAME
###                         "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/array_partition_io.cpp.in"
###                     OUTPUT_PATHNAME
###                         "${output_pathname}"
###                     DICTIONARY
###                         '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
###                 )
###                 list(APPEND generated_source_files "${output_pathname}")
###             endforeach()
###         endforeach()
###     endforeach()
###
###     set(generated_source_files ${generated_source_files} PARENT_SCOPE)
### endblock()

# / Instantiate components -----------------------------------------------------


add_library(lue_framework_algorithm INTERFACE)
add_library(lue::framework_algorithm ALIAS lue_framework_algorithm)

set_target_properties(lue_framework_algorithm
    PROPERTIES
        EXPORT_NAME framework_algorithm
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

target_sources(lue_framework_algorithm
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
)

target_link_libraries(lue_framework_algorithm
    INTERFACE
        lue::framework_partitioned_array
        lue::framework_partitioned_route
        lue::framework_core
        HPX::hpx
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_algorithm
)

lue_install_development_libraries(
    TARGETS
        lue_framework_algorithm
)


# Instantiate local operations -------------------------------------------------
set(generated_source_files "")
set(offset "source/local_operation")

block()
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
            string(REPLACE "::" "_" element ${Element})

            foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)

                # Instantiate add
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/add-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/equal_to-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/greater_than-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/greater_than_equal_to-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/less_than-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/less_than_equal_to-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/not_equal_to-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/valid-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/valid.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate modulus
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/modulus-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/modulus.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate multiply
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/multiply-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/subtract-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/where2-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/where2.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","ConditionElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate where3
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/where3-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/where3.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","ConditionElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()


block()
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS
                LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS
                LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS
            )
            string(REPLACE "::" "_" element ${Element})

            foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)

                # Instantiate abs
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/abs-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/abs.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate negate
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/negate-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/negate.cpp.in"
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
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS
                LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS
            )
            string(REPLACE "::" "_" element ${Element})

            foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)

                # acos
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/acos-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/asin-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/atan-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/atan2-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/close_to-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/close_to.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","ExpressionElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # ceil
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/ceil-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/ceil.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # cos
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/cos-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/divide-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/exp-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/exp.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # floor
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/floor-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/floor.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # log
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/log-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/log.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # log10
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/log10-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/log10.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # pow
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/pow-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/pow.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # round
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/round-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/round.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # sin
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/sin-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/sqrt-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/tan-${Policies}_${element}_${rank}.cpp")

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
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(OutputElement IN LISTS LUE_FRAMEWORK_ELEMENTS)
            string(REPLACE "::" "_" output_element ${OutputElement})

            foreach(InputElement IN LISTS LUE_FRAMEWORK_ELEMENTS)
                string(REPLACE "::" "_" input_element ${InputElement})

                foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)

                    # Instantiate cast
                    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/cast-${Policies}_${output_element}_${input_element}_${rank}.cpp")

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

    set(IndexElement ${LUE_FRAMEWORK_INDEX_ELEMENT})
    set(ConditionElement ${LUE_FRAMEWORK_BOOLEAN_ELEMENT})

    string(REPLACE "::" "_" index_element ${IndexElement})
    string(REPLACE "::" "_" condition_element ${ConditionElement})

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)

            # Instantiate cell_index
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/cell_index-${Policies}_${index_element}_${condition_element}_${rank}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/cell_index.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","IndexElement":"${IndexElement}","ConditionElement":"${ConditionElement}","rank":"${rank}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            string(REPLACE "::" "_" element ${Element})

            foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)

                # Instantiate logical_and
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/logical_and-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/logical_exclusive_or-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/logical_inclusive_or-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/logical_not-${Policies}_${element}_${rank}.cpp")

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
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/unique_id-${Policies}_${element}_${rank}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/unique_id.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","ConditionElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","IDElement":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()
# / Instantiate local operations -----------------------------------------------

add_library(lue_framework_local_operation SHARED)
add_library(lue::framework_local_operation ALIAS lue_framework_local_operation)

set_target_properties(lue_framework_local_operation
    PROPERTIES
        EXPORT_NAME framework_local_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_local_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/local_operation_export.hpp
    EXPORT_MACRO_NAME LUE_LOCAL_OPERATION_EXPORT
)

target_sources(lue_framework_local_operation
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/local_operation_export.hpp
    PRIVATE
        ${generated_source_files}
)

target_link_libraries(lue_framework_local_operation
    PUBLIC
        lue::framework_algorithm
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_local_operation
)

lue_install_development_libraries(
    TARGETS
        lue_framework_local_operation
)


# Instantiate focal operations -------------------------------------------------
set(generated_source_files "")
set(offset "source/focal_operation")

block()
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
            string(REPLACE "::" "_" element ${Element})

            # Instantiate focal_maximum
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_maximum-${Policies}_${element}.cpp")

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
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_minimum-${Policies}_${element}.cpp")

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
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_sum-${Policies}_${element}.cpp")

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
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            string(REPLACE "::" "_" element ${Element})

            # Instantiate aspect
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/aspect-${Policies}_${element}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/aspect.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate convolve
            set(Weight ${Element})
            set(weight ${element})
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/convolve-${Policies}_${element}_${weight}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/convolve.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}","Weight":"${Weight}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate focal_high_pass
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_high_pass-${Policies}_${element}.cpp")

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
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_mean-${Policies}_${element}.cpp")

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
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/gradients-${Policies}_${element}.cpp")

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
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/slope-${Policies}_${element}.cpp")

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
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            string(REPLACE "::" "_" element ${Element})

            # Instantiate focal_majority
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_majority-${Policies}_${element}.cpp")

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
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            string(REPLACE "::" "_" element ${Element})

            # Instantiate focal_diversity
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/focal_diversity-${Policies}_${element}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/focal_diversity.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                '{"Policies":"${Policies}","Count":"${LUE_FRAMEWORK_COUNT_ELEMENT}","Element":"${Element}","Boolean":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()
# /Instantiate focal operations ------------------------------------------------


add_library(lue_framework_focal_operation SHARED)
add_library(lue::framework_focal_operation ALIAS lue_framework_focal_operation)

set_target_properties(lue_framework_focal_operation
    PROPERTIES
        EXPORT_NAME framework_focal_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_focal_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/focal_operation_export.hpp
    EXPORT_MACRO_NAME LUE_FOCAL_OPERATION_EXPORT
)

target_sources(lue_framework_focal_operation
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/focal_operation_export.hpp
    PRIVATE
        ${generated_source_files}
)

target_link_libraries(lue_framework_focal_operation
    PUBLIC
        lue::framework_algorithm
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_focal_operation
)

lue_install_development_libraries(
    TARGETS
        lue_framework_focal_operation
)


# Instantiate miscellaneous operations -------------------------------------------------
set(generated_source_files "")
set(offset "source/miscellaneous_operation")

block()
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
            string(REPLACE "std::" "" element ${Element})

            # Instantiate resample
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/resample-${Policies}_${element}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/resample.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            if(NOT Element MATCHES "std::u?int8_t")
                # Instantiate uniform
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/uniform-${Policies}_${element}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/uniform.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                foreach(SomeElement IN LISTS LUE_FRAMEWORK_ELEMENTS)
                    string(REPLACE "std::" "" some_element ${SomeElement})

                    # Instantiate uniform with template
                    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/uniform-${Policies}_${element}_${some_element}.cpp")

                    generate_template_instantiation(
                        INPUT_PATHNAME
                            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/uniform_template.cpp.in"
                        OUTPUT_PATHNAME
                            "${output_pathname}"
                        DICTIONARY
                            '{"Policies":"${Policies}","Element":"${Element}","SomeElement":"${SomeElement}"}'
                    )
                    list(APPEND generated_source_files "${output_pathname}")
                endforeach()
            endif()
        endforeach()

        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            string(REPLACE "std::" "" element ${Element})

            # Instantiate normal
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/normal-${Policies}_${element}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/normal.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            foreach(SomeElement IN LISTS LUE_FRAMEWORK_ELEMENTS)
                string(REPLACE "std::" "" some_element ${SomeElement})

                # Instantiate normal with template
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/normal-${Policies}_${element}_${some_element}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/normal_template.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","SomeElement":"${SomeElement}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()
# /Instantiate miscellaneous operations ----------------------------------------


add_library(lue_framework_miscellaneous_operation SHARED)
add_library(lue::framework_miscellaneous_operation ALIAS lue_framework_miscellaneous_operation)

set_target_properties(lue_framework_miscellaneous_operation
    PROPERTIES
        EXPORT_NAME framework_miscellaneous_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_miscellaneous_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/miscellaneous_operation_export.hpp
    EXPORT_MACRO_NAME LUE_MISCELLANEOUS_OPERATION_EXPORT
)

target_sources(lue_framework_miscellaneous_operation
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/miscellaneous_operation_export.hpp
    PRIVATE
        ${generated_source_files}
)

target_link_libraries(lue_framework_miscellaneous_operation
    PUBLIC
        lue::framework_algorithm
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_miscellaneous_operation
)

lue_install_development_libraries(
    TARGETS
        lue_framework_miscellaneous_operation
)


# Instantiate zonal operations -------------------------------------------------
set(generated_source_files "")

block()
    # Instantiate clump
    set(count "0")
    set(rank "2")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Zone IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
            math(EXPR count "${count} + 1")

            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/zonal_operation/clump-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/source/zonal_operation/clump.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Zone":"${Zone}","rank":"${rank}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_area
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Zone IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
            math(EXPR count "${count} + 1")

            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/zonal_operation/zonal_area-${count}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/source/zonal_operation/zonal_area.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","Count":"${LUE_FRAMEWORK_COUNT_ELEMENT}","Zone":"${Zone}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_diversity
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            foreach(Zone IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/zonal_operation/zonal_diversity-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/source/zonal_operation/zonal_diversity.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Count":"${LUE_FRAMEWORK_COUNT_ELEMENT}","Element":"${Element}","Zone":"${Zone}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    # Instantiate zonal_majority
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            foreach(Zone IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/zonal_operation/zonal_majority-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/source/zonal_operation/zonal_majority.cpp.in"
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
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
            foreach(Zone IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/zonal_operation/zonal_maximum-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/source/zonal_operation/zonal_maximum.cpp.in"
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
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            foreach(Zone IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/zonal_operation/zonal_mean-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/source/zonal_operation/zonal_mean.cpp.in"
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
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
            foreach(Zone IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/zonal_operation/zonal_minimum-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/source/zonal_operation/zonal_minimum.cpp.in"
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
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            foreach(Zone IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/zonal_operation/zonal_normal-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/source/zonal_operation/zonal_normal.cpp.in"
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
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
            foreach(Zone IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/zonal_operation/zonal_sum-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/source/zonal_operation/zonal_sum.cpp.in"
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
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            foreach(Zone IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
                math(EXPR count "${count} + 1")

                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/zonal_operation/zonal_uniform-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/source/zonal_operation/zonal_uniform.cpp.in"
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


add_library(lue_framework_zonal_operation SHARED)
add_library(lue::framework_zonal_operation ALIAS lue_framework_zonal_operation)

set_target_properties(lue_framework_zonal_operation
    PROPERTIES
        EXPORT_NAME framework_zonal_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_zonal_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/zonal_operation_export.hpp
    EXPORT_MACRO_NAME LUE_ZONAL_OPERATION_EXPORT
)

target_sources(lue_framework_zonal_operation
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/zonal_operation_export.hpp
    PRIVATE
        ${generated_source_files}
)

target_link_libraries(lue_framework_zonal_operation
    PUBLIC
        lue::framework_algorithm
        lue::framework_global_operation  # unique
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_zonal_operation
)

lue_install_development_libraries(
    TARGETS
        lue_framework_zonal_operation
)


# Instantiate global operations ------------------------------------------------
set(generated_source_files "")
set(offset "source/global_operation")


block()
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)
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
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
            foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)
                math(EXPR count "${count} + 1")

                # Instantiate maximum
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/maximum-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/maximum.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate minimum
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/minimum-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/minimum.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","Element":"${Element}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate sum
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/sum-${count}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/sum.cpp.in"
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
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(InputElement IN LISTS LUE_FRAMEWORK_INTEGRAL_ELEMENTS)
            foreach(OutputElement IN LISTS LUE_FRAMEWORK_ELEMENTS)
                foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)
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


add_library(lue_framework_global_operation SHARED)
add_library(lue::framework_global_operation ALIAS lue_framework_global_operation)

set_target_properties(lue_framework_global_operation
    PROPERTIES
        EXPORT_NAME framework_global_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_global_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/global_operation_export.hpp
    EXPORT_MACRO_NAME LUE_GLOBAL_OPERATION_EXPORT
)

target_sources(lue_framework_global_operation
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/global_operation_export.hpp
    PRIVATE
        ${generated_source_files}
)

target_link_libraries(lue_framework_global_operation
    PUBLIC
        lue::framework_algorithm
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_global_operation
)

lue_install_development_libraries(
    TARGETS
        lue_framework_global_operation
)


# Instantiate routing operations -----------------------------------------------
set(generated_source_files "")
set(offset "source/routing_operation")


block()
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)

        if(LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS)
            # Instantiate accu_info
            string(REPLACE "::" "_" name ${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT})
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu_info-${Policies}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/accu_info.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"name:":"${name}","Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endif()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_MATERIAL_ELEMENTS)
            string(REPLACE "::" "_" element ${Element})

            # Instantiate downstream
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/downstream-${element}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/downstream.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate upstream
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/upstream-${element}.cpp")

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
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            string(REPLACE "::" "_" element ${Element})

            # Instantiate accu
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu-${Policies}_${element}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/accu.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"name":"${element}","Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            if(LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS)
                # Instantiate partial_accu
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/partial_accu-${Policies}_${element}.cpp")

                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/partial_accu.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"name":"${element}","Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endif()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

block()
    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(Element IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
            string(REPLACE "::" "_" element ${Element})

            # Instantiate accu_fraction
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu_fraction-${Policies}_${element}.cpp")

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
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/accu_threshold-${Policies}_${element}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/accu_threshold.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"name":"${element}","Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            # Instantiate d8_flow_direction
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/d8_flow_direction-${Policies}_${element}.cpp")

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
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/kinematic_wave-${Policies}_${element}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/kinematic_wave.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"name":"${element}","Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()


block()
    set(count "0")
    set(rank "2")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(ZoneElement IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
            foreach(InputElement IN LISTS LUE_FRAMEWORK_ELEMENTS)
                math(EXPR count "${count} + 1")

                # Instantiate decreasing_order (global)
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/decreasing_order-${count}.cpp")
                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/decreasing_order.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","OutputElement":"${ZoneElement}","InputElement":"${InputElement}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                math(EXPR count "${count} + 1")

                # Instantiate decreasing_order (zonal)
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/decreasing_order-${count}.cpp")
                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/decreasing_order.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","ZoneElement":"${ZoneElement}","InputElement":"${InputElement}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        foreach(OutputElement IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
            foreach(ZoneElement IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
                math(EXPR count "${count} + 1")

                # Instantiate first_n
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/first_n-${count}.cpp")
                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/first_n.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                    '{"Policies":"${Policies}","OutputElement":"${OutputElement}","RouteID":"${ZoneElement}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()


block()
    set(count "0")

    foreach(Element IN LISTS LUE_FRAMEWORK_MATERIAL_ELEMENTS)
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

# block()
#     foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
#         foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
#             string(REPLACE "::" "_" element ${Element})
#
#             # Instantiate downstream
#             set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/downstream-${element}.cpp")
#
#             generate_template_instantiation(
#                 INPUT_PATHNAME
#                     "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/downstream.cpp.in"
#                 OUTPUT_PATHNAME
#                     "${output_pathname}"
#                 DICTIONARY
#                     '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${Element}"}'
#             )
#             list(APPEND generated_source_files "${output_pathname}")
#         endforeach()
#     endforeach()
#
#     set(generated_source_files ${generated_source_files} PARENT_SCOPE)
# endblock()

block()
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
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
    set(count "0")

    foreach(Policies IN LISTS LUE_FRAMEWORK_ALGORITHM_POLICIES)
        math(EXPR count "${count} + 1")

        if(LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS)
            # Instantiate inflow_count
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/inflow_count-${count}.cpp")

            # We're using the boolean element type for the counts, assuming the boolean element type is a small
            # integral capable of representing small numbers.
            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/inflow_count.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                '{"Policies":"${Policies}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","Element":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endif()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()
# / Instantiate routing operations ---------------------------------------------


add_library(lue_framework_routing_operation SHARED)
add_library(lue::framework_routing_operation ALIAS lue_framework_routing_operation)

set_target_properties(lue_framework_routing_operation
    PROPERTIES
        EXPORT_NAME framework_routing_operation
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_routing_operation
    EXPORT_FILE_NAME include/lue/framework/algorithm/routing_operation_export.hpp
    EXPORT_MACRO_NAME LUE_ROUTING_OPERATION_EXPORT
)

target_sources(lue_framework_routing_operation
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/routing_operation_export.hpp
    PRIVATE
        ${generated_source_files}
        source/routing_operation/channel.cpp
        source/routing_operation/walk/cell_counter.cpp
)

target_link_libraries(lue_framework_routing_operation
    PUBLIC
        lue::framework_algorithm
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_routing_operation
)

lue_install_development_libraries(
    TARGETS
        lue_framework_routing_operation
)

# ------------------------------------------------------------------------------

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
