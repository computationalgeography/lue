#pragma once
#include "lue/imgui/glfw/frame.hpp"


namespace lue::imgui::glfw {

    class VulkanFrame: public Frame
    {

        public:

            VulkanFrame(lue::glfw::Window& window);

            ~VulkanFrame() override;
    };

}  // namespace lue::imgui::glfw
