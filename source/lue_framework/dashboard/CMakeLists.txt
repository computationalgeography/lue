add_subdirectory(imgui)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

set(doxygen_input ${doxygen_input} PARENT_SCOPE)
