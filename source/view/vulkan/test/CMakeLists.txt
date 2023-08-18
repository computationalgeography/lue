set(scope lue_vulkan)
set(names
    buffer_test
    command_buffer_test
    command_pool_test
    debug_report_callback_test
    descriptor_pool_test
    descriptor_set_layout_test
    device_test
    fence_test
    framebuffer_test
    image_view_test
    instance_test
    memory_test
    physical_device_test
    pipeline_layout_test
    pipeline_test
    queue_test
    render_pass_test
    semaphore_test
    shader_module_test
    surface_test
    swapchain_test
)

foreach(name ${names})
    set(test_name ${scope}_${name})

    add_executable(${test_name} ${name}.cpp)

    target_link_libraries(${test_name}
        PRIVATE
            lue::vulkan
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND ${test_name}
    )
endforeach()
