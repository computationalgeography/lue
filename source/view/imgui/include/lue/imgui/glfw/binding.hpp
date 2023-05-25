#pragma once
#include <imgui.h>


namespace lue::imgui::glfw {

    class Binding
    {

        public:

            static ImGuiIO& io()
            {
                return ImGui::GetIO();
            }

            virtual ~Binding();

        protected:

            Binding();
    };

}  // namespace lue::imgui::glfw
