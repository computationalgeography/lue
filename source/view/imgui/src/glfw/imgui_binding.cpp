#include "lue/imgui/glfw/imgui_binding.hpp"
#include "lue/imgui/font/IconsFontAwesome5.h"
#include "lue/imgui/font/fa-solid-900.hpp"
#include "lue/imgui/font/roboto-medium.hpp"
#include <array>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdexcept>


namespace lue::imgui::glfw {

    ImGuiBinding::ImGuiBinding(std::string const& glsl_version, lue::glfw::Window& window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();  // (void)io;
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version.c_str());

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


    ImGuiBinding::~ImGuiBinding()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

}  // namespace lue::imgui::glfw
