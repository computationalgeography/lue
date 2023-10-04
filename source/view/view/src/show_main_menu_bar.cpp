#include "lue/view/show_main_menu_bar.hpp"
#include "lue/git.hpp"
#include "lue/glfw.hpp"
#include "lue/imgui.hpp"
#include "lue/version.hpp"


namespace lue::view {

    namespace {

        void show_about_window(bool* show_about)
        {
            ImGui::OpenPopup("About...");

            if (ImGui::BeginPopupModal("About...", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("LUE version %s (build %s)", version().c_str(), short_sha1().c_str());
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

    }  // Anonymous namespace


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

}  // namespace lue::view
