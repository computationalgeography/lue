list(APPEND doxygen_input
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/doc
)
set(doxygen_input ${doxygen_input} PARENT_SCOPE)

add_test_conditionally(test)

add_library(lue_vulkan STATIC
    src/application_info.cpp
    src/buffer.cpp
    src/command_buffer.cpp
    src/command_pool.cpp
    src/debug_report_callback.cpp
    src/descriptor_set.cpp
    src/descriptor_pool.cpp
    src/descriptor_set_layout.cpp
    src/device.cpp
    src/error.cpp
    src/fence.cpp
    src/framebuffer.cpp
    src/image_view.cpp
    src/instance.cpp
    src/memory.cpp
    src/physical_device.cpp
    src/pipeline.cpp
    src/pipeline_layout.cpp
    src/queue.cpp
    src/queue_family.cpp
    src/render_pass.cpp
    src/semaphore.cpp
    src/shader_module.cpp
    src/surface.cpp
    src/swapchain.cpp
)

target_include_directories(lue_vulkan
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(lue::vulkan ALIAS lue_vulkan)

target_link_libraries(lue_vulkan
    PUBLIC
        fmt::fmt
        Vulkan::Vulkan
)
