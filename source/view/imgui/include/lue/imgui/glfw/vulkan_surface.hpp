#pragma once
#include "lue/glfw/window.hpp"
#include "lue/vulkan/instance.hpp"
#include <vulkan/vulkan.h>


namespace lue::imgui::glfw {

    class VulkanSurface
    {

        public:

            VulkanSurface(vulkan::Instance const& instance, lue::glfw::Window& window);

            ~VulkanSurface();

        private:

            VkInstance _instance;

            VkSurfaceKHR _surface;
    };

}  // namespace lue::imgui::glfw
