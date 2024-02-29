list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/doc)

add_subdirectory(cxx)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/cxx)

add_subdirectory(python)
add_subdirectory(script)

set(doxygen_input ${doxygen_input} PARENT_SCOPE)
