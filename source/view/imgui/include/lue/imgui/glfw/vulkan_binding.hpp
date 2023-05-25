#pragma once
#include "lue/imgui/glfw/binding.hpp"
#include "lue/glfw/window.hpp"
#include <imgui.h>


namespace lue::imgui::glfw {

    class VulkanBinding: public Binding
    {

        public:

            VulkanBinding(lue::glfw::Window& window);

            ~VulkanBinding() override;
    };

}  // namespace lue::imgui::glfw
