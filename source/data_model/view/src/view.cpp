#include "view.hpp"
#include "dataset_to_visualize.hpp"
#include "lue/configure.hpp"
#include "lue/gui.hpp"
#include <fmt/format.h>
#include <boost/filesystem.hpp>
#include <iostream>


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

} // Anonymous namespace


namespace lue {
namespace utility {
namespace {

// Or just use a optional<lue::Dataset> ?
using DatasetsToVisualize = std::vector<DatasetToVisualize>;


void show_about_window(
    bool* show_about)
{
    ImGui::OpenPopup("About...");

    if(ImGui::BeginPopupModal(
            "About...", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("LUE version %s", LUE_VERSION);
        ImGui::Separator();
        ImGui::Text("By the PCRaster research and development team");
        ImGui::Text("LUE is licensed under the GPL-2 License");
        ImGui::Text("More info at https://www.pcraster.eu");

        if(ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            *show_about = false;
        }
        ImGui::EndPopup();
    }
}


void show_main_menu_bar()
{
    static bool show_about = false;
#ifndef NDEBUG
    static bool show_imgui_demo = false;
#endif

    if(ImGui::BeginMainMenuBar()) {

        if(ImGui::BeginMenu("File")) {
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("View")) {
#ifndef NDEBUG
            ImGui::MenuItem("ImGui Demo", nullptr, &show_imgui_demo);
#endif
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("About...", nullptr, &show_about);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

#ifndef NDEBUG
    if(show_imgui_demo) {
        ImGui::ShowDemoWindow(&show_imgui_demo);
    }
#endif

    if(show_about) {
        show_about_window(&show_about);
    }
}


void show_array(
    Array const& /* array */)
{
}


template<
    typename Value>
void               show_value          (Value const& value);


template<>
void show_value<same_shape::Value>(
    same_shape::Value const& value)
{
    show_array(dynamic_cast<Array const&>(value));
}


template<>
void show_value<same_shape::constant_shape::Value>(
    same_shape::constant_shape::Value const& value)
{
    show_array(dynamic_cast<Array const&>(value));
}


template<>
void show_value<same_shape::variable_shape::Value>(
    same_shape::variable_shape::Value const& /* value */)
{
}


template<>
void show_value<different_shape::Value>(
    different_shape::Value const& /* value */)
{
}


template<>
void show_value<different_shape::constant_shape::Value>(
    different_shape::constant_shape::Value const& /* value */)
{
}


template<>
void show_value<different_shape::variable_shape::Value>(
    different_shape::variable_shape::Value const& /* value */)
{
}


void show_object_id(
    ObjectID const& /* object_id */)
{
}


void show_time_domain(
    TimeDomain const& /* domain */)
{
}


void show_space_domain(
    SpaceDomain const& /* domain */)
{
}


template<
    typename Property>
void               show_property       (Property const& property);


template<>
void show_property<same_shape::Property>(
    same_shape::Property const& property)
{
    show_value(property.value());
}


template<>
void show_property<same_shape::constant_shape::Property>(
    same_shape::constant_shape::Property const& property)
{
    show_value(property.value());
}


template<>
void show_property<same_shape::variable_shape::Property>(
    same_shape::variable_shape::Property const& property)
{
    show_value(property.value());
}


template<>
void show_property<different_shape::Property>(
    different_shape::Property const& property)
{
    show_value(property.value());
}


template<>
void show_property<different_shape::constant_shape::Property>(
    different_shape::constant_shape::Property const& property)
{
    show_value(property.value());
}


template<>
void show_property<different_shape::variable_shape::Property>(
    different_shape::variable_shape::Property const& property)
{
    show_value(property.value());
}


template<
    typename Collection>
void show_properties(
    Properties const& properties)
{
    Collection const& collection{properties.collection<Collection>()};

    for(std::string const& name: collection.names()) {
        if(ImGui::TreeNode(name.c_str())) {
            show_property(collection[name]);
            ImGui::TreePop();
        }
    }
}


void show_property_set(
    PropertySet const& property_set)
{
    if(ImGui::TreeNode("time domain")) {
        if(property_set.has_time_domain()) {
            auto const& domain = property_set.time_domain();
            show_time_domain(domain);
        }

        ImGui::TreePop();
    }

    if(ImGui::TreeNode("space domain")) {
        if(property_set.has_space_domain()) {
            auto const& domain = property_set.space_domain();
            show_space_domain(domain);
        }

        ImGui::TreePop();
    }

    {
        auto const& properties = property_set.properties();

        if(ImGui::TreeNode(fmt::format(
                "properties ({})", properties.size()).c_str())) {

            show_properties<same_shape::Properties>(properties);
            show_properties<same_shape::constant_shape::Properties>(
                properties);
            show_properties<same_shape::variable_shape::Properties>(
                properties);

            show_properties<different_shape::Properties>(properties);
            show_properties<different_shape::constant_shape::Properties>(
                properties);
            show_properties<different_shape::variable_shape::Properties>(
                properties);

            ImGui::TreePop();
        }
    }
}


void show_phenomenon(
    Phenomenon const& phenomenon)
{
    {
        auto const& object_id{phenomenon.object_id()};

        if(ImGui::TreeNode(fmt::format(
                "object_id ({})", object_id.nr_objects()).c_str())) {

            show_object_id(object_id);
            ImGui::TreePop();
        }
    }

    {
        auto const& property_sets{phenomenon.collection_property_sets()};

        if(ImGui::TreeNode(fmt::format(
                "collection property-sets ({})",
                property_sets.size()).c_str())) {

            for(std::string const& name: property_sets.names()) {
                if(ImGui::TreeNode(name.c_str())) {
                    show_property_set(property_sets[name]);
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }

    {
        auto const& property_sets{phenomenon.property_sets()};

        if(ImGui::TreeNode(fmt::format(
                "property-sets ({})", property_sets.size()).c_str())) {

            for(std::string const& name: property_sets.names()) {
                if(ImGui::TreeNode(name.c_str())) {
                    show_property_set(property_sets[name]);
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }
}


void show_phenomena(
    Phenomena const& phenomena)
{
    for(std::string const& name: phenomena.names()) {
        if(ImGui::TreeNode(name.c_str())) {
            show_phenomenon(phenomena[name]);
            ImGui::TreePop();
        }
    }
}


void show_universe(
    Universe const& universe)
{
    if(ImGui::TreeNode("phenomena")) {
        show_phenomena(universe.phenomena());
        ImGui::TreePop();
    }
}


void show_universes(
    Universes const& universes)
{
    for(std::string const& name: universes.names()) {
        if(ImGui::TreeNode(name.c_str())) {
            show_universe(universes[name]);
            ImGui::TreePop();
        }
    }
}

}  // Anonymous namespace


View::View(
    std::vector<std::string> const& arguments)

    : Application(usage, arguments)

{
    // Turn off error stack traversal. The default functions prints
    // lots of messages we usually don't care about.
    H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);
}


int View::run_implementation()
{
    // Provide the user with an interface with which to browse the
    // contents of the dataset. Hide the HDF5 specific stuff used in
    // the LUE data model as much as possible.
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
    //         - Hide HDF5 stuff
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
    // - Assume dataset is valid
    // - Handle live changing datasets gracefully

    auto const dataset_names = argument<std::vector<std::string>>("<dataset>");

    // Open datasets
    DatasetsToVisualize datasets_to_visualize{
            dataset_names.begin(), dataset_names.end()
        };

    sdl2::API api;
    sdl2::Window sdl_window{"LUE view"};
    sdl2::ImGuiBinding binding{sdl_window};

    bool stop_browsing = false;

    while(!stop_browsing) {

        // Handle key presses
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT) {
                stop_browsing = true;
            }
        }

        sdl2::imgui::Frame frame{sdl_window};

        show_main_menu_bar();

        sdl2::imgui::Window imgui_window{"Datasets"};

        for(auto const& dataset: datasets_to_visualize) {
            auto const& source = dataset.dataset();
            auto const dataset_path =
                boost::filesystem::canonical(source.pathname());
            auto const dataset_pathname = dataset_path.string();
            auto const dataset_filename = dataset_path.filename().string();
            auto const dataset_parent_pathname =
                dataset_path.parent_path().string();

            if(ImGui::CollapsingHeader(dataset_filename.c_str())) {

                ImGui::PushID(dataset_pathname.c_str());

                ImGui::TextUnformatted(dataset_pathname.c_str());

                if(ImGui::TreeNode(fmt::format(
                            "phenomena ({})",
                            source.phenomena().size()
                        ).c_str())) {
                    show_phenomena(source.phenomena());
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode(fmt::format(
                            "universes ({})",
                            source.universes().size()
                        ).c_str())) {
                    show_universes(source.universes());
                    ImGui::TreePop();
                }

                ImGui::PopID();
            }
        }
    }

    return EXIT_SUCCESS;
}

}  // namespace utility
}  // namespace lue
