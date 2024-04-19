list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/doc)

add_subdirectory(cxx)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/cxx)

# if(LUE_FRAMEWORK_WITH_PYTHON_API)
#     add_subdirectory(python)
#     list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/python)
# endif()
#
# set(doxygen_input ${doxygen_input} PARENT_SCOPE)
