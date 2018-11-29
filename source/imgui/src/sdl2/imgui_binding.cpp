#include "lue/gui/sdl2/imgui_binding.hpp"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_sdl.h"


namespace sdl2 {

ImGuiBinding::ImGuiBinding(
    Window& window)
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // io = ImGui::GetIO();
    // (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    ImGui_ImplSDL2_InitForOpenGL(window, window.gl_context());
    ImGui_ImplOpenGL2_Init();

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
}


ImGuiBinding::~ImGuiBinding()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}


ImGuiIO& ImGuiBinding::io()
{
    return ImGui::GetIO();
}

}  // namespace sdl2
