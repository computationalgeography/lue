add_library(lue_vulkan STATIC
    src/application_info.cpp
    src/debug_report_callback.cpp
    src/error.cpp
    src/instance.cpp
    src/physical_device.cpp
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
