#include "lue/view/opengl_application.hpp"
#include "lue/view/show_dataset.hpp"
#include "lue/glfw.hpp"
#include "lue/imgui.hpp"
#include <cmath>


namespace lue::view {

    OpenGLApplication::OpenGLApplication(std::vector<std::string> const& arguments):

        Application{arguments}

    {
    }


    OpenGLApplication::~OpenGLApplication()
    {
    }


    int OpenGLApplication::run_implementation()
    {
        auto const dataset_names = argument<std::vector<std::string>>("<dataset>");

        glfw::Library library{};
        glfw::Monitor monitor{};

        // Decide GL+GLSL versions
#ifdef __APPLE__
        // GL 3.2 + GLSL 150
        std::string const glsl_version{"#version 150"};
        glfw::Window::hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfw::Window::hint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfw::Window::hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfw::Window::hint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
        // GL 3.0 + GLSL 130
        std::string const glsl_version{"#version 130"};
        glfw::Window::hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfw::Window::hint(GLFW_CONTEXT_VERSION_MINOR, 0);
        // glfw::Window::hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        // glfw::Window::hint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

        glfw::VideoMode const video_mode{monitor.video_mode()};
        glfw::Window::hint(GLFW_RED_BITS, video_mode.red_bits());
        glfw::Window::hint(GLFW_GREEN_BITS, video_mode.green_bits());
        glfw::Window::hint(GLFW_BLUE_BITS, video_mode.blue_bits());
        glfw::Window::hint(GLFW_REFRESH_RATE, video_mode.refresh_rate());

        glfw::Window glfw_window{
            "LUE view",
            static_cast<int>(std::ceil(0.9 * video_mode.width())),
            static_cast<int>(std::ceil(0.9 * video_mode.height()))};
        glfw_window.make_context_current();

        // TODO This must be moved elsewhere
        glfwSwapInterval(1);  // Enable vsync

        imgui::glfw::OpenGLBinding binding{glsl_version, glfw_window};

        // static std::string const ini_pathname{
        //     expand_environment_variables("${HOME}/.lue_view.ini")};
        // binding.io().IniFilename = ini_pathname.c_str();
        imgui::glfw::Binding::io().IniFilename = nullptr;  // Skip for now

        // Open datasets
        Datasets datasets_to_visualize{dataset_names.begin(), dataset_names.end()};

        // TODO How to keep the loop efficient. When nothing changes,
        // the loop should not take a lot of resources...

        bool quit = glfw_window.should_close();
        // ImVec4 const clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        while (!quit)
        {
            glfw::Library::poll_events();

            // Draw stuff on window
            // TODO(KDJ)

            imgui::glfw::OpenGLFrame frame{glfw_window};

            static Configuration configuration{};
            quit = show_main_menu_bar(configuration);

            if (!quit)
            {
                show_datasets(datasets_to_visualize, configuration.show_details());
            }

            /// // Rendering
            /// ImGui::Render();
            /// auto const [display_width, display_height] = glfw_window.framebuffer_size();
            /// glViewport(0, 0, display_width, display_height);
            /// glClearColor(
            ///     clear_color.x * clear_color.w,
            ///     clear_color.y * clear_color.w,
            ///     clear_color.z * clear_color.w,
            ///     clear_color.w);
            /// glClear(GL_COLOR_BUFFER_BIT);
            /// ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /// glfwSwapBuffers(glfw_window);

            quit = quit || glfw_window.should_close();
        }

        return EXIT_SUCCESS;
    }

}  // namespace lue::view
