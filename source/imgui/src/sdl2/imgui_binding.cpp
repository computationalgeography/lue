#include "lue/gui/sdl2/imgui_binding.hpp"
#include "lue/gui/font/fa-solid-900.hpp"
#include "lue/gui/font/IconsFontAwesome5.h"
#include "lue/gui/font/roboto-medium.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <GL/glew.h>    // Initialize with glewInit()
#include <array>
#include <stdexcept>


namespace sdl2 {

ImGuiBinding::ImGuiBinding(
    API const& api,
    Window& window)
{
    if(glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize OpenGL loader!");
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();  // (void)io;
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, window.gl_context());
    ImGui_ImplOpenGL3_Init(api.glsl_version().c_str());

    io.Fonts->AddFontFromMemoryCompressedTTF(
        roboto_medium_compressed_data, roboto_medium_compressed_size,
        20.0);

    ImFontConfig config;
    config.MergeMode = true;
    // Use if you want to make the icon monospaced
    config.GlyphMinAdvanceX = 20.0;
    // config.PixelSnapH = true;

    // Will not be copied by AddFont* so keep in scope.
    static std::array<ImWchar, 3> const icon_ranges{ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromMemoryCompressedTTF(
        fa_solid_900_compressed_data, fa_solid_900_compressed_size,
        16.0, &config, icon_ranges.data());
}


ImGuiBinding::~ImGuiBinding()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

}  // namespace sdl2
