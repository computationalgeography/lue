add_library(lue_vulkan STATIC
    src/application_info.cpp
    src/extension.cpp
    src/instance.cpp
    src/instance_create_info.cpp
    src/queue_family.cpp
)

target_include_directories(lue_vulkan
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(lue::vulkan ALIAS lue_vulkan)

target_link_libraries(lue_vulkan
    PUBLIC
        Vulkan::Vulkan
)
