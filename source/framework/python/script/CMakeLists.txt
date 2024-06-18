file(GENERATE
    OUTPUT ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/lue_calculate.py
    INPUT lue_calculate.py
)


lue_install_programs(TARGETS
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/lue_calculate.py
)
