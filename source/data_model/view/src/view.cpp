#include "view.hpp"
#include "view_configuration.hpp"
#include "dataset_to_visualize.hpp"
#include "lue/configure.hpp"
#include "lue/gui.hpp"
#include <fmt/format.h>
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

template<
    typename Properties>
class PropertiesTraits;


template<>
class PropertiesTraits<same_shape::Properties>
{

public:

    static constexpr ShapePerObject shape_per_object{ShapePerObject::same};

    static constexpr ValueVariability value_variability{
        ValueVariability::constant};

    static constexpr ShapeVariability shape_variability{
        ShapeVariability::constant};

};


template<>
class PropertiesTraits<same_shape::constant_shape::Properties>
{

public:

    static constexpr ShapePerObject shape_per_object{ShapePerObject::same};

    static constexpr ValueVariability value_variability{
        ValueVariability::variable};

    static constexpr ShapeVariability shape_variability{
        ShapeVariability::constant};

};


template<>
class PropertiesTraits<same_shape::variable_shape::Properties>
{

public:

    static constexpr ShapePerObject shape_per_object{ShapePerObject::same};

    static constexpr ValueVariability value_variability{
        ValueVariability::variable};

    static constexpr ShapeVariability shape_variability{
        ShapeVariability::variable};

};


template<>
class PropertiesTraits<different_shape::Properties>
{

public:

    static constexpr ShapePerObject shape_per_object{ShapePerObject::different};

    static constexpr ValueVariability value_variability{
        ValueVariability::constant};

    static constexpr ShapeVariability shape_variability{
        ShapeVariability::constant};

};


template<>
class PropertiesTraits<different_shape::constant_shape::Properties>
{

public:

    static constexpr ShapePerObject shape_per_object{ShapePerObject::different};

    static constexpr ValueVariability value_variability{
        ValueVariability::variable};

    static constexpr ShapeVariability shape_variability{
        ShapeVariability::constant};

};


template<>
class PropertiesTraits<different_shape::variable_shape::Properties>
{

public:

    static constexpr ShapePerObject shape_per_object{ShapePerObject::different};

    static constexpr ValueVariability value_variability{
        ValueVariability::variable};

    static constexpr ShapeVariability shape_variability{
        ShapeVariability::variable};

};


template<
    typename Properties>
static constexpr ShapePerObject shape_per_object =
    PropertiesTraits<Properties>::shape_per_object;


template<
    typename Properties>
static constexpr ValueVariability value_variability =
    PropertiesTraits<Properties>::value_variability;


template<
    typename Properties>
static constexpr ShapeVariability shape_variability =
    PropertiesTraits<Properties>::shape_variability;


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


void show_main_menu_bar(
    ViewConfiguration& configuration)
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
            ImGui::MenuItem(
                "Show details", nullptr, &configuration.show_details());
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


void show_identifier(
    hdf5::Identifier const& identifier)
{
    ImGui::TextUnformatted(fmt::format(
        "HDF5 identifier: {}", static_cast<hid_t>(identifier)).c_str());
    // TODO type()
    // TODO info()
    ImGui::TextUnformatted(fmt::format(
        "is_valid: {}", identifier.is_valid()).c_str());
    ImGui::TextUnformatted(fmt::format(
        "pathname: {}", identifier.pathname()).c_str());
    ImGui::TextUnformatted(fmt::format(
        "name: {}", identifier.name()).c_str());
}


void show_primary_data_object(
    hdf5::PrimaryDataObject const& primary_data_object)
{
    show_identifier(primary_data_object.id());
    // TODO attributes: requires to be able to query them
}


void show_group(
    hdf5::Group const& group)
{
    show_primary_data_object(
        dynamic_cast<hdf5::PrimaryDataObject const&>(group));
}


void show_file(
    hdf5::File const& file)
{
    show_group(dynamic_cast<hdf5::Group const&>(file));
}


void show_array(
    Array const& /* array */)
{
}


template<
    typename Value>
void               show_value          (Value const& value,
                                        bool const show_details);


template<>
void show_value<same_shape::Value>(
    same_shape::Value const& value,
    bool const /* show_details */)
{
    show_array(dynamic_cast<Array const&>(value));
}


template<>
void show_value<same_shape::constant_shape::Value>(
    same_shape::constant_shape::Value const& value,
    bool const /* show_details */)
{
    show_array(dynamic_cast<Array const&>(value));
}


template<>
void show_value<same_shape::variable_shape::Value>(
    same_shape::variable_shape::Value const& /* value */,
    bool const /* show_details */)
{
}


template<>
void show_value<different_shape::Value>(
    different_shape::Value const& /* value */,
    bool const /* show_details */)
{
}


template<>
void show_value<different_shape::constant_shape::Value>(
    different_shape::constant_shape::Value const& /* value */,
    bool const /* show_details */)
{
}


template<>
void show_value<different_shape::variable_shape::Value>(
    different_shape::variable_shape::Value const& /* value */,
    bool const /* show_details */)
{
}


void show_object_id(
    ObjectID const& object_id,
    bool const show_details)
{
    show_value(dynamic_cast<same_shape::Value const&>(object_id), show_details);
}


void show_object_tracker(
    ObjectTracker const& /* object_tracker */,
    bool const /* show_details */)
{
}


std::string epoch_to_string(
    time::Epoch const epoch)
{
    std::string result;

    if(!epoch.origin()) {
        result = aspect_to_string(epoch.kind());
    }
    else {
        if(!epoch.calendar()) {
            result = fmt::format(
                "{} / {}",
                aspect_to_string(epoch.kind()),
                *epoch.origin());
        }
        else {
            result = fmt::format(
                "{} / {} / {}",
                aspect_to_string(epoch.kind()),
                *epoch.origin(),
                aspect_to_string(*epoch.calendar()));
        }
    }

    return result;
}


void show_time_domain(
    TimeDomain const& domain,
    bool const /* show_details */)
{
    ImGui::Indent();

    // Configuration
    {
        auto const& configuration{domain.configuration()};

        ImGui::Text("item type: ");
        ImGui::SameLine();
        ImGui::Text(aspect_to_string(
            configuration.value<TimeDomainItemType>()).c_str());
    }

    // Clock
    {
        auto const& clock{domain.clock()};

        ImGui::Text("epoch: ");
        ImGui::SameLine();
        ImGui::Text(epoch_to_string(clock.epoch()).c_str());

        ImGui::Text("unit: ");
        ImGui::SameLine();
        ImGui::Text(aspect_to_string(clock.unit()).c_str());

        ImGui::Text("nr_units: ");
        ImGui::SameLine();
        ImGui::Text(fmt::format("{}", clock.nr_units()).c_str());
    }

    // Value
    {
        // FIXME
        ImGui::Text("value: ");
        ImGui::SameLine();
        ImGui::Text("TODO");
    }

    ImGui::Unindent();
}


void show_space_domain(
    SpaceDomain const& domain,
    bool const /* show_details */)
{
    ImGui::Indent();

    // Configuration
    {
        auto const& configuration{domain.configuration()};

        ImGui::Text("mobility: ");
        ImGui::SameLine();
        ImGui::Text(aspect_to_string(
            configuration.value<Mobility>()).c_str());

        ImGui::Text("item type: ");
        ImGui::SameLine();
        ImGui::Text(aspect_to_string(
            configuration.value<SpaceDomainItemType>()).c_str());
    }

    // Discretized presence property
    {
        if(domain.presence_is_discretized()) {
            ImGui::Text("discretized presence property: ");
            ImGui::SameLine();
            ImGui::Text(
                const_cast<SpaceDomain&>(domain)
                    .discretized_presence_property().id().pathname().c_str());
        }
    }

    // Value
    {
        // FIXME
        ImGui::Text("value: ");
        ImGui::SameLine();
        ImGui::Text("TODO");
    }

    ImGui::Unindent();
}


template<
    typename Property>
void               show_property       (Property const& property,
                                        bool const show_details);


template<>
void show_property<same_shape::Property>(
    same_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<>
void show_property<same_shape::constant_shape::Property>(
    same_shape::constant_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<>
void show_property<same_shape::variable_shape::Property>(
    same_shape::variable_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<>
void show_property<different_shape::Property>(
    different_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<>
void show_property<different_shape::constant_shape::Property>(
    different_shape::constant_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<>
void show_property<different_shape::variable_shape::Property>(
    different_shape::variable_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<
    typename Collection>
void show_properties(
    Properties const& properties,
    bool const show_details)
{
    Collection const& collection{properties.collection<Collection>()};

    if(!collection.empty()) {

        ImGui::Indent();
        ImGui::BeginGroup();

        ImGui::Text("shape per object: ");
        ImGui::SameLine();
        ImGui::Text(aspect_to_string(shape_per_object<Collection>).c_str());

        ImGui::Text("value variability: ");
        ImGui::SameLine();
        ImGui::Text(aspect_to_string(value_variability<Collection>).c_str());

        ImGui::Text("shape variability: ");
        ImGui::SameLine();
        ImGui::Text(aspect_to_string(shape_variability<Collection>).c_str());

        ImGui::Separator();
        ImGui::EndGroup();
        ImGui::Unindent();

        for(std::string const& name: collection.names()) {
            if(ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

                if(ImGui::BeginPopupContextItem()) {
                    if(ImGui::MenuItem("Copy name")) {
                        ImGui::LogToClipboard();
                        ImGui::LogText(name.c_str());
                        ImGui::LogFinish();
                    }
                    ImGui::EndPopup();
                }

                show_property(collection[name], show_details);
                ImGui::TreePop();
            }
        }
    }
}


void show_property_set(
    PropertySet const& property_set,
    bool const show_details)
{
    if(property_set.has_time_domain()) {
        if(ImGui::TreeNodeEx("object tracker")) {
            show_object_tracker(property_set.object_tracker(), show_details);
            ImGui::TreePop();
        }

        if(ImGui::TreeNodeEx("time domain", ImGuiTreeNodeFlags_DefaultOpen)) {
            show_time_domain(property_set.time_domain(), show_details);
            ImGui::TreePop();
        }
    }

    if(property_set.has_space_domain()) {
        if(ImGui::TreeNodeEx("space domain", ImGuiTreeNodeFlags_DefaultOpen)) {
            show_space_domain(property_set.space_domain(), show_details);
            ImGui::TreePop();
        }
    }

    {
        auto const& properties = property_set.properties();

        if(ImGui::TreeNodeEx(fmt::format(
                "properties ({})", properties.size()).c_str(),
                ImGuiTreeNodeFlags_DefaultOpen)) {

            show_properties<same_shape::Properties>(
                properties, show_details);
            show_properties<same_shape::constant_shape::Properties>(
                properties, show_details);
            show_properties<same_shape::variable_shape::Properties>(
                properties, show_details);

            show_properties<different_shape::Properties>(
                properties, show_details);
            show_properties<different_shape::constant_shape::Properties>(
                properties, show_details);
            show_properties<different_shape::variable_shape::Properties>(
                properties, show_details);

            ImGui::TreePop();
        }
    }
}


void show_property_sets(
    PropertySets const& property_sets,
    bool const show_details)
{
    for(std::string const& name: property_sets.names()) {
        if(ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

            if(ImGui::BeginPopupContextItem()) {
                if(ImGui::MenuItem("Copy name")) {
                    ImGui::LogToClipboard();
                    ImGui::LogText(name.c_str());
                    ImGui::LogFinish();
                }
                ImGui::EndPopup();
            }

            show_property_set(property_sets[name], show_details);
            ImGui::TreePop();
        }
    }
}


void show_phenomenon(
    Phenomenon const& phenomenon,
    bool const show_details)
{
    {
        auto const& object_id{phenomenon.object_id()};

        if(show_details || object_id.nr_objects() > 0) {
            if(ImGui::TreeNodeEx(fmt::format(
                    "object_id ({})", object_id.nr_objects()).c_str())) {

                show_object_id(object_id, show_details);
                ImGui::TreePop();
            }
        }
    }

    {
        auto const& property_sets{phenomenon.collection_property_sets()};

        if(show_details || !property_sets.empty()) {
            if(ImGui::TreeNodeEx(fmt::format(
                    "collection property-sets ({})",
                        property_sets.size()).c_str(),
                    ImGuiTreeNodeFlags_DefaultOpen)) {

                show_property_sets(property_sets, show_details);
                ImGui::TreePop();
            }
        }
    }

    {
        auto const& property_sets{phenomenon.property_sets()};

        if(show_details || !property_sets.empty()) {
            if(ImGui::TreeNodeEx(fmt::format(
                    "property-sets ({})", property_sets.size()).c_str(),
                    ImGuiTreeNodeFlags_DefaultOpen)) {

                show_property_sets(property_sets, show_details);
                ImGui::TreePop();
            }
        }
    }
}


void show_phenomena(
    Phenomena const& phenomena,
    bool const show_details)
{
    for(std::string const& name: phenomena.names()) {

        if(ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

            if(ImGui::BeginPopupContextItem()) {
                if(ImGui::MenuItem("Copy name")) {
                    ImGui::LogToClipboard();
                    ImGui::LogText(name.c_str());
                    ImGui::LogFinish();
                }
                ImGui::EndPopup();
            }

            show_phenomenon(phenomena[name], show_details);

            ImGui::TreePop();
        }
    }
}


void show_universe(
    Universe const& universe,
    bool const show_details)
{
    if(ImGui::TreeNodeEx("phenomena", ImGuiTreeNodeFlags_DefaultOpen)) {
        show_phenomena(universe.phenomena(), show_details);
        ImGui::TreePop();
    }
}


void show_universes(
    Universes const& universes,
    bool const show_details)
{
    for(std::string const& name: universes.names()) {
        if(ImGui::TreeNodeEx(name.c_str())) {

            if(ImGui::BeginPopupContextItem()) {
                if(ImGui::MenuItem("Copy name")) {
                    ImGui::LogToClipboard();
                    ImGui::LogText(name.c_str());
                    ImGui::LogFinish();
                }
                ImGui::EndPopup();
            }

            show_universe(universes[name], show_details);
            ImGui::TreePop();

        }
    }
}


void show_dataset(
    DatasetToVisualize& dataset,
    bool const show_details)
{
    auto const& source{dataset.dataset()};

    if(show_details) {
        show_file(dynamic_cast<hdf5::File const&>(source));
    }

    {
        auto const& phenomena{source.phenomena()};

        if(show_details || !phenomena.empty()) {
            if(ImGui::TreeNodeEx(
                    fmt::format("phenomena ({})", phenomena.size()).c_str(),
                    ImGuiTreeNodeFlags_DefaultOpen)) {

                show_phenomena(phenomena, show_details);
                ImGui::TreePop();
            }
        }
    }

    {
        auto const& universes{source.universes()};

        if(show_details || !universes.empty()) {
            if(ImGui::TreeNodeEx(
                    fmt::format("universes ({})", universes.size()).c_str(),
                    ImGuiTreeNodeFlags_DefaultOpen)) {

                show_universes(universes, show_details);
                ImGui::TreePop();
            }
        }
    }
}


void show_datasets(
    DatasetsToVisualize& datasets,
    bool const show_details)
{
    // Window for presenting information about the loaded datasets
    sdl2::imgui::Window imgui_window{"Datasets"};

    ImGuiTabBarFlags tab_bar_flags{ImGuiTabBarFlags_None};

    if(ImGui::BeginTabBar("Datasets", tab_bar_flags)) {

        for(auto& dataset: datasets) {

            if(ImGui::BeginTabItem(dataset.filename().c_str())) {

                if(ImGui::BeginPopupContextItem()) {
                    if(ImGui::MenuItem("Copy name")) {
                        ImGui::LogToClipboard();
                        ImGui::LogText(dataset.filename().c_str());
                        ImGui::LogFinish();
                    }
                    ImGui::EndPopup();
                }

                dataset.rescan();

                if(dataset.is_open()) {
                    show_dataset(dataset, show_details);
                }

                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
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

    // Open datasets
    DatasetsToVisualize datasets_to_visualize{
            dataset_names.begin(), dataset_names.end()
        };

    sdl2::API api;
    sdl2::Window sdl_window{"LUE view",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600};
    sdl2::ImGuiBinding binding{api, sdl_window};

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

        // Draw stuff on window
        // TODO

        sdl2::imgui::Frame frame{sdl_window};

        static ViewConfiguration configuration{};
        show_main_menu_bar(configuration);
        show_datasets(datasets_to_visualize, configuration.show_details());
    }

    return EXIT_SUCCESS;
}

}  // namespace utility
}  // namespace lue
