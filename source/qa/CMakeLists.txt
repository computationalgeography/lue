add_subdirectory(cxx)
add_subdirectory(python)

file(GENERATE
    OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/lue_scalability.py
    INPUT lue_scalability.py
)
