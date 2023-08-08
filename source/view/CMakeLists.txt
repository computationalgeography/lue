list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/glfw)
add_subdirectory(glfw)

if(LUE_VIEW_USE_VULKAN)
    list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/vulkan)
    add_subdirectory(vulkan)
endif()

if(LUE_BUILD_IMGUI)
    list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
    add_subdirectory(imgui)
endif()

list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/view)
add_subdirectory(view)

set(doxygen_input ${doxygen_input} PARENT_SCOPE)
