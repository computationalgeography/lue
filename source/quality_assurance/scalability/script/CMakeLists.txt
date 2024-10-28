set(names
    array_shape
    scalability
)

foreach(name ${names})
    file(GENERATE
        OUTPUT ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/lue_${name}.py
        INPUT lue_${name}.py
    )

    lue_install_programs(TARGETS
        ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/lue_${name}.py
    )
endforeach()
