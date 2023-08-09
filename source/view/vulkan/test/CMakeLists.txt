set(scope lue_vulkan)
set(names
    command_pool_test
    debug_report_callback_test
    device_test
    fence_test
    framebuffer_test
    image_view_test
    instance_test
    pipeline_test
    pipeline_layout_test
    physical_device_test
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
