add_subdirectory(glfw)

if(LUE_VIEW_USE_VULKAN)
    add_subdirectory(vulkan)
endif()

if(LUE_BUILD_IMGUI)
    add_subdirectory(imgui)
endif()

add_subdirectory(view)
