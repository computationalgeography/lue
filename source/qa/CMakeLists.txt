list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/doc)

add_subdirectory(cxx)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/cxx)

if(LUE_QA_WITH_PYTHON_API)
    add_subdirectory(python)
endif()

add_subdirectory(scalability)

set(doxygen_input ${doxygen_input} PARENT_SCOPE)
