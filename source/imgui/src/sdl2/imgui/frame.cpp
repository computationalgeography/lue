#include "lue/gui/sdl2/imgui/frame.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <SDL_opengl.h>


namespace sdl2 {
namespace imgui {

Frame::Frame(
    sdl2::Window& window):

    _window{window}

{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(_window);
    ImGui::NewFrame();
}


Frame::~Frame()
{
    // Rendering
    ImGui::Render();

    auto& io = ImGui::GetIO();
    glViewport(
        0, 0,
        static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(_window);
}

}  // namespace imgui
}  // namespace sdl2
