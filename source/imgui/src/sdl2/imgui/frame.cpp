#include "lue/gui/sdl2/imgui/frame.hpp"
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_sdl.h"
#include <SDL_opengl.h>


namespace sdl2 {
namespace imgui {

Frame::Frame(
    sdl2::Window& window)
:
    _window{window}
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(_window);
    ImGui::NewFrame();
}


Frame::~Frame()
{
    // Rendering
    ImGui::Render();

    auto& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(_window);

    glClear(GL_COLOR_BUFFER_BIT);
}

}  // namespace imgui
}  // namespace sdl2
