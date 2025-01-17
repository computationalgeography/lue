add_library(lue_vulkan STATIC
    source/application_info.cpp
    source/debug_report_callback.cpp
    source/error.cpp
    source/instance.cpp
    source/physical_device.cpp
    source/queue_family.cpp
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
