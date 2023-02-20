#pragma once
#include "lue/gui/glfw/api.hpp"
#include "lue/gui/glfw/window.hpp"
#include <imgui.h>


namespace glfw {

    class ImGuiBinding
    {

        public:

            ImGuiBinding(API const& api, Window& window);

            ImGuiBinding(ImGuiBinding const&) = default;

            ImGuiBinding(ImGuiBinding&&) = default;

            ~ImGuiBinding();

            ImGuiBinding& operator=(ImGuiBinding const&) = default;

            ImGuiBinding& operator=(ImGuiBinding&&) = default;

            static ImGuiIO& io()
            {
                return ImGui::GetIO();
            }
    };

}  // namespace glfw
