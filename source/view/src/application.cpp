#include "lue/view/application.hpp"
#include "lue/view/configuration.hpp"
#include "lue/view/dataset.hpp"
#include "lue/configure.hpp"
#include "lue/git.hpp"
#include "lue/gui.hpp"
#include <fmt/format.h>


namespace lue::view {
    namespace {

        std::string const usage = fmt::format(R"(
View LUE datasets

usage:
    {0}
    {0} <dataset>...
    {0} (-h | --help) | --version

options:
    -h --help   Show this screen
    --version   Show version
)", "lue_view");


        void show_about_window(
            bool* show_about)
        {
            ImGui::OpenPopup("About...");

            if(ImGui::BeginPopupModal(
                    "About...", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("LUE version %s (build %s)",
                    BuildOptions::version, Git::short_sha1);
                ImGui::Separator();
                ImGui::Text("By the Computational Geography research and development team");
                ImGui::Text("LUE is licensed under the MIT License");
                ImGui::Text("More info at https://lue.computationalgeography.org");

                if(ImGui::Button("Close")) {
                    ImGui::CloseCurrentPopup();
                    *show_about = false;
                }
                ImGui::EndPopup();
            }
        }


        bool show_main_menu_bar(
            Configuration& configuration)
        {
            static bool show_about = false;
#ifndef NDEBUG
            static bool show_imgui_demo = false;
#endif

            bool quit{false};

            if(ImGui::BeginMainMenuBar())
            {
                if(auto file_menu = gui::Menu("File"))
                {
                    // TODO
                    // - open file

                    if(ImGui::MenuItem("Quit"))
                    {
                        quit = true;
                    }
                }

                if(auto view_menu = gui::Menu("View"))
                {
                    ImGui::MenuItem(
                        "Show details",
                        // configuration.show_details()
                            // ? ICON_FA_EYE_SLASH "Show details"
                            // : ICON_FA_EYE "Show details",
                        nullptr, &configuration.show_details());
#ifndef NDEBUG
                    ImGui::MenuItem("ImGui Demo", nullptr, &show_imgui_demo);
#endif
                }

                if(auto help_menu = gui::Menu("Help"))
                {
                    ImGui::MenuItem("About...", nullptr, &show_about);
                }

                ImGui::EndMainMenuBar();
            }

#ifndef NDEBUG
            if(show_imgui_demo)
            {
                ImGui::ShowDemoWindow(&show_imgui_demo);
            }
#endif

            if(show_about)
            {
                show_about_window(&show_about);
            }

            return quit;
        }

    }  // Anonymous namespace


    void show_datasets(
        Datasets& datasets,
        bool const show_details);


    Application::Application(
        std::vector<std::string> const& arguments):

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

        glfw::API api;
        glfw::Window glfw_window{"LUE view", 1000, 800};
        glfw::ImGuiBinding binding{api, glfw_window};

        // static std::string const ini_pathname{
        //     expand_environment_variables("${HOME}/.lue_view.ini")};
        // binding.io().IniFilename = ini_pathname.c_str();
        glfw::ImGuiBinding::io().IniFilename = nullptr;  // Skip for now

        // Open datasets
        Datasets datasets_to_visualize{dataset_names.begin(), dataset_names.end()};

        bool quit = glfwWindowShouldClose(glfw_window);

        // TODO How to keep the loop efficient. When nothing changes,
        // the loop should not take a lot of resources...

        while(!quit)
        {
            glfwPollEvents();

            // Draw stuff on window
            // TODO(KDJ)

            glfw::imgui::Frame frame{glfw_window};

            static Configuration configuration{};
            quit = show_main_menu_bar(configuration);

            if(!quit)
            {
                show_datasets(datasets_to_visualize, configuration.show_details());
            }

            quit = quit || glfwWindowShouldClose(glfw_window);
        }

        return EXIT_SUCCESS;
    }

}  // namespace lue::view
