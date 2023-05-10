#include "lue/view/application.hpp"
#include "lue/view/configuration.hpp"
#include "lue/view/dataset.hpp"
#include "lue/configure.hpp"
#include "lue/git.hpp"
#include "lue/glfw.hpp"
#include "lue/imgui.hpp"
// #include "imgui_impl_opengl3.h"
#include <fmt/format.h>


namespace lue::view {
    namespace {

        std::string const usage = fmt::format(
            R"(
View LUE datasets

usage:
    {0}
    {0} <dataset>...
    {0} (-h | --help) | --version

options:
    -h --help   Show this screen
    --version   Show version
)",
            "lue_view");


        void show_about_window(bool* show_about)
        {
            ImGui::OpenPopup("About...");

            if (ImGui::BeginPopupModal("About...", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("LUE version %s (build %s)", BuildOptions::version, Git::short_sha1);
                ImGui::Separator();
                ImGui::Text("By the Computational Geography research and development team");
                ImGui::Text("LUE is licensed under the MIT License");
                ImGui::Text("More info at https://lue.computationalgeography.org");
                ImGui::Separator();
                auto const [major, minor, revision] = glfw::Library::version();
                ImGui::Text(
                    "GLFW: %s / %d.%d.%d", glfw::Library::version_string().c_str(), major, minor, revision);

                if (ImGui::Button("Close"))
                {
                    ImGui::CloseCurrentPopup();
                    *show_about = false;
                }
                ImGui::EndPopup();
            }
        }


        bool show_main_menu_bar(Configuration& configuration)
        {
            static bool show_about = false;
#ifndef NDEBUG
            static bool show_imgui_demo = false;
#endif

            bool quit{false};

            if (ImGui::BeginMainMenuBar())
            {
                if (auto file_menu = imgui::Menu("File"))
                {
                    // TODO
                    // - open file

                    if (ImGui::MenuItem("Quit"))
                    {
                        quit = true;
                    }
                }

                if (auto view_menu = imgui::Menu("View"))
                {
                    ImGui::MenuItem(
                        "Show details",
                        // configuration.show_details()
                        // ? ICON_FA_EYE_SLASH "Show details"
                        // : ICON_FA_EYE "Show details",
                        nullptr,
                        &configuration.show_details());
#ifndef NDEBUG
                    ImGui::MenuItem("ImGui Demo", nullptr, &show_imgui_demo);
#endif
                }

                if (auto help_menu = imgui::Menu("Help"))
                {
                    ImGui::MenuItem("About...", nullptr, &show_about);
                }

                ImGui::EndMainMenuBar();
            }

#ifndef NDEBUG
            if (show_imgui_demo)
            {
                ImGui::ShowDemoWindow(&show_imgui_demo);
            }
#endif

            if (show_about)
            {
                show_about_window(&show_about);
            }

            return quit;
        }

    }  // Anonymous namespace


    void show_datasets(Datasets& datasets, bool const show_details);


    Application::Application(std::vector<std::string> const& arguments):

        utility::Application{usage, arguments}

    {
        // Turn off error stack traversal. The default functions prints
        // lots of messages we usually don't care about.
        H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);
    }


    int Application::run_implementation()
    {
        // - Visualize property values
        //     - time domain
        //         - time boxes
        //         - time cells
        //         - time points
        //     - space domain
        //         - space points
        //             - points map
        //         - space boxes
        //             - boxes map
        //     - id
        //     - properties
        //         - time series graph
        //         - raster map
        //         - scalars table
        // - Interface
        //     - Navigator, showing an overview (layout, hierarchy) of a dataset
        //     - View, visualizing a piece of information
        //         - time domain
        //         - space domain
        //         - property
        // - Make it possible to visualize multiple pieces of information at the
        //   same time. If they are part of the same property-set, set up
        //   a link/cursor/... E.g. selecting a space point should show the
        //   time series graph of a selected property for that point.
        //   Also set up links between stuff in the same phenomenon. Set up links
        //   between as much stuff as possible.

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

        imgui::glfw::ImGuiBinding binding{glsl_version, glfw_window};

        // static std::string const ini_pathname{
        //     expand_environment_variables("${HOME}/.lue_view.ini")};
        // binding.io().IniFilename = ini_pathname.c_str();
        imgui::glfw::ImGuiBinding::io().IniFilename = nullptr;  // Skip for now

        // Open datasets
        Datasets datasets_to_visualize{dataset_names.begin(), dataset_names.end()};

        // TODO How to keep the loop efficient. When nothing changes,
        // the loop should not take a lot of resources...

        bool quit = glfwWindowShouldClose(glfw_window);
        // ImVec4 const clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        while (!quit)
        {
            glfwPollEvents();

            // Draw stuff on window
            // TODO(KDJ)

            imgui::glfw::Frame frame{glfw_window};

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

            quit = quit || glfwWindowShouldClose(glfw_window);
        }

        return EXIT_SUCCESS;
    }

}  // namespace lue::view
