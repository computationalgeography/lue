#pragma once
#include "lue/glfw/window.hpp"
#include <imgui.h>


namespace lue::imgui::glfw {

    class ImGuiBinding
    {

        public:

            ImGuiBinding(std::string const& glsl_version, lue::glfw::Window& window);

            ~ImGuiBinding();

            static ImGuiIO& io()
            {
                return ImGui::GetIO();
            }
    };

}  // namespace lue::imgui::glfw
