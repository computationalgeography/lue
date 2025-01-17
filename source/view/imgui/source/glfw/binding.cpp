#include "lue/imgui/glfw/binding.hpp"
#include "lue/imgui/font/IconsFontAwesome5.h"
#include "lue/imgui/font/fa-solid-900.hpp"
#include "lue/imgui/font/roboto-medium.hpp"
#include <array>
#include <imgui_impl_glfw.h>


namespace lue::imgui::glfw {

    Binding::Binding()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        io.Fonts->AddFontFromMemoryCompressedTTF(
            roboto_medium_compressed_data, roboto_medium_compressed_size, 20.0);

        ImFontConfig config;
        config.MergeMode = true;
        // Use if you want to make the icon monospaced
        config.GlyphMinAdvanceX = 20.0;
        // config.PixelSnapH = true;

        // Will not be copied by AddFont* so keep in scope.
        static std::array<ImWchar, 3> const icon_ranges{ICON_MIN_FA, ICON_MAX_FA, 0};
        io.Fonts->AddFontFromMemoryCompressedTTF(
            fa_solid_900_compressed_data, fa_solid_900_compressed_size, 16.0, &config, icon_ranges.data());
    }


    Binding::~Binding()
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

}  // namespace lue::imgui::glfw
