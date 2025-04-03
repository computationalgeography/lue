add_library(lue_vulkan STATIC)
add_library(lue::vulkan ALIAS lue_vulkan)

target_sources(lue_vulkan
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/lue
    PRIVATE
        source/application_info.cpp
        source/debug_report_callback.cpp
        source/error.cpp
        source/instance.cpp
        source/physical_device.cpp
        source/queue_family.cpp
)

target_link_libraries(lue_vulkan
    PUBLIC
        lue::compile_features
        Vulkan::Vulkan
)
