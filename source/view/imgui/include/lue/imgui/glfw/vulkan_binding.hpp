#pragma once
#include "lue/imgui/glfw/binding.hpp"
#include "lue/glfw/window.hpp"
#include "lue/vulkan.hpp"
#include <imgui.h>


namespace lue::imgui::glfw {

    class VulkanBinding: public Binding
    {

        public:

            VulkanBinding(
                lue::glfw::Window& window,
                vulkan::Instance& instance,
                vulkan::PhysicalDevice& physical_device,
                vulkan::Device& device,
                vulkan::Queue& queue,
                VkPipelineCache pipeline_cache,
                int const nr_images);

            ~VulkanBinding() override;
    };

}  // namespace lue::imgui::glfw
