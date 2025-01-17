#include "lue/imgui/glfw/vulkan_surface.hpp"
#include "lue/vulkan/error.hpp"


namespace lue::imgui::glfw {

    VulkanSurface::VulkanSurface(vulkan::Instance const& instance, lue::glfw::Window& window):

        _instance{instance},
        _surface{}


    {
        VkResult result{::glfwCreateWindowSurface(instance, window, nullptr, &_surface)};
        vulkan::assert_result_is_ok(result);
    }


    VulkanSurface::~VulkanSurface()
    {
        ::vkDestroySurfaceKHR(_instance, _surface, nullptr);
    }

}  // namespace lue::imgui::glfw
