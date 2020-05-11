#include "lue/view/application.hpp"
#include "lue/view/configuration.hpp"
#include "lue/view/dataset.hpp"
#include "lue/configure.hpp"
#include "lue/gui.hpp"
// #include "lue/utility/environment.hpp"
#include <fmt/format.h>
#include <functional>


// void ImGui::PlotHistogram(
//     const char* label,
//     const float* values,
//     int values_count,
//     int values_offset,
//     const char* overlay_text,
//     float scale_min,
//     float scale_max,
//     ImVec2 graph_size,
//     int stride)

// void ImGui::PlotHistogram(
//     const char* label,
//     float (*values_getter)(void* data, int idx),
//     void* data,
//     int values_count,
//     int values_offset,
//     const char* overlay_text,
//     float scale_min,
//     float scale_max,
//     ImVec2 graph_size)

// ImGui::PlotHistogram("My title", array, nr_elements, 0, NULL, 0.0f, 1.0f, ImVec2(0,80));
//
// - Read values as floats (progress bar)
// - Determine min/max/mean/std. Show in table.
// - Optionally show histogram


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
namespace data_model {

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

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays,-warnings-as-errors)
    static constexpr char label[] = {"same shape"};

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

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays,-warnings-as-errors)
    static constexpr char label[] = {"same constant shape"};

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

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays,-warnings-as-errors)
    static constexpr char label[] = {"same variable shape"};

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

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays,-warnings-as-errors)
    static constexpr char label[] = {"different shape"};

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

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays,-warnings-as-errors)
    static constexpr char label[] = {"different constant shape"};

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

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays,-warnings-as-errors)
    static constexpr char label[] = {"different variable shape"};

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


// template<
//     typename Properties>
// static const std::string label{PropertiesTraits<Properties>::label};

}  // namespace data_model


namespace view {
namespace {

// template<
//     typename Object,
//     typename DataModelObject,
//     typename ObjectRetriever>
// Object& use_cache(
//     Cache& cache,
//     DataModelObject const& data_model_object,
//     ObjectRetriever const& retriever)
// {
//     // Key used by the cache
//     hdf5::Identifier const& id{data_model_object.id()};
// 
//     // If no object is stored for this key, use the retriever to obtain
//     // one and store it in the cache
//     if(!cache.contains(id)) {
//         cache.insert<Object>(id, retriever(data_model_object));
//     }
// 
//     assert(cache.contains(id));
// 
//     // Return object stored in the cache
//     return cache.retrieve<Object>(id);
// }


// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.txt)
void help_marker(
    std::string const& description)
{
    static int const nr_characters{20};

    ImGui::TextDisabled(ICON_FA_INFO);

    if(ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * nr_characters);
        ImGui::TextUnformatted(description.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


void copy_popup(
    std::string const& label,
    std::string const& string)
{
    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::MenuItem((ICON_FA_COPY "Copy " + label).c_str())) {
            ImGui::LogToClipboard();
            ImGui::LogText(string.c_str());
            ImGui::LogFinish();
        }
        ImGui::EndPopup();
    }
}


std::string shape_as_string(
    hdf5::Shape const& shape)
{
    std::stringstream stream;
    stream << "(";

    // Prevent seperator at end of string.
    if(!shape.empty()) {
        std::copy(shape.begin(), shape.end() - 1,
            std::ostream_iterator<typename hdf5::Shape::value_type>(stream,
                ", "));
        stream << shape.back();
    }
    stream << ")";

    return stream.str();
}


// Or just use a optional<lue::Dataset> ?
using Datasets = std::vector<Dataset>;


void show_about_window(
    bool* show_about)
{
    ImGui::OpenPopup("About...");

    if(ImGui::BeginPopupModal(
            "About...", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("LUE version %s (build %s)",
            BuildOptions::version, BuildOptions::git_short_sha1);
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
    Configuration& configuration)
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
                "Show details",
                // configuration.show_details()
                    // ? ICON_FA_EYE_SLASH "Show details"
                    // : ICON_FA_EYE "Show details",
                nullptr, &configuration.show_details());
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
    // TODO(KDJ) type()
    // TODO(KDJ) info()
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
    // TODO(KDJ) attributes: requires to be able to query them
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
    ImGui::Text("HDF5 version: ");
    ImGui::SameLine();
    ImGui::Text(file.hdf5_version().c_str());
    ImGui::SameLine();
    help_marker("The version of HDF5 used to create the file");

    show_group(dynamic_cast<hdf5::Group const&>(file));
}


void show_file_datatype(
    hdf5::Datatype const& datatype)
{
    ImGui::Text("file datatype: ");
    ImGui::SameLine();
    ImGui::Text(hdf5::standard_datatype_as_string(datatype).c_str());
    ImGui::SameLine();
    help_marker("The type used to store the array elements in the file.");
}


void show_memory_datatype(
    hdf5::Datatype const& datatype)
{
    ImGui::Text("memory datatype: ");
    ImGui::SameLine();
    ImGui::Text(hdf5::native_datatype_as_string(datatype).c_str());
    ImGui::SameLine();
    help_marker("The type used to store the array elements in memory.");
}


void show_rank(
    data_model::Rank const rank)
{
    ImGui::Text("rank: ");
    ImGui::SameLine();
    ImGui::Text(fmt::format("{}", rank).c_str());
    ImGui::SameLine();
    help_marker("The rank of each array stored per object.");
}


void show_nr_objects(
    data_model::Count const nr_objects)
{
    ImGui::Text("nr objects: ");
    ImGui::SameLine();
    ImGui::Text(fmt::format("{}", nr_objects).c_str());
    ImGui::SameLine();
    help_marker("The number of objects for which arrays are stored.");
}


void show_dataset(
    hdf5::Dataset const& /* dataset */)
{
}


void show_array(
    data_model::Array const& array,
    bool const show_details)
{
    show_file_datatype(array.file_datatype());

    if(show_details) {
        show_memory_datatype(array.memory_datatype());
        show_dataset(array);
    }
}


void show_value_group(
    data_model::ValueGroup const& value,
    bool const show_details)
{
    show_file_datatype(value.file_datatype());

    if(show_details) {
        show_memory_datatype(value.memory_datatype());
    }
}


template<
    typename Value>
void               show_value          (Value const& value,
                                        bool show_details);


std::string const array_shape_doc{
    "Shape of an object array. Each number is an extent along a "
    "dimension. An empty shape implies a scalar value."};


template<>
void show_value<data_model::same_shape::Value>(
    data_model::same_shape::Value const& value,
    bool const show_details)
{
    ImGui::Text("nr arrays: ");
    ImGui::SameLine();
    ImGui::Text(fmt::format("{}", value.nr_arrays()).c_str());
    ImGui::SameLine();
    help_marker("For each object an nD array is stored.");

    ImGui::Text("array shape: ");
    ImGui::SameLine();
    ImGui::Text(shape_as_string(value.array_shape()).c_str());
    ImGui::SameLine();
    help_marker(array_shape_doc);

    show_array(dynamic_cast<data_model::Array const&>(value), show_details);

    data_model::Rank const rank{
        static_cast<data_model::Rank>(value.array_shape().size())};

    // Visualize values. Options:
    // - If 0D values:
    //     - Table with value per object. Limit the size of the table.
    //     - If numeric values:
    //         - Table with summary statistics
    //         - Histogram with distribution of values
    //
    // Tabs with visualizations?
    // When reading values, cache them as long as the dataset is not
    // updated. Invalidate cache once the dataset is updated.

    if constexpr(BuildOptions::build_framework)
    {
        if(auto tab_bar = gui::TabBar("Values"))
        {
            // Read values, showing progress bar while doing it
            // Do this asynchronously and cache results. Visualize values
            // once the data has arrived. Don't show them in the meantime.
            // Invalidate the cache if rescanning the datasets detects
            // changes to the dataset. Use the ID as a unique identifier
            // of the property value in the cache.
            //
            // cache[ID] = async(read);
            //
            // Maybe cache per kind of property value? Static cache in
            // this function? Well then we can't invalidate the cache.
            // TODO(KDJ)


            if(rank == 0) {

                // Table with for each object its value
                if(auto tab_item = gui::TabItem("Table"))
                {
                    // ImGui::Indent();

                        ImGui::Columns(2, "mycolumns"); // 4-ways, with border
                        ImGui::Separator();
                        ImGui::Text("ID"); ImGui::NextColumn();
                        ImGui::Text("Value"); ImGui::NextColumn();
                        ImGui::Separator();

                        // Iterate over all objects and add ID and value to table
                        // TODO(KDJ)



                        // const char* names[3] = { "One", "Two", "Three" };
                        // const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
                        // static int selected = -1;
                        // for (int i = 0; i < 3; i++)
                        // {
                        //     char label[32];
                        //     sprintf(label, "%04d", i);
                        //     if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
                        //         selected = i;
                        //     bool hovered = ImGui::IsItemHovered();
                        //     ImGui::NextColumn();
                        //     ImGui::Text(names[i]); ImGui::NextColumn();
                        //     ImGui::Text(paths[i]); ImGui::NextColumn();
                        //     ImGui::Text("%d", hovered); ImGui::NextColumn();
                        // }

                        ImGui::Columns(1);  // Reset nr_cols
                        ImGui::Separator();

                    // ImGui::Unindent();
                }
            }
        }
    }
}


template<>
void show_value<data_model::same_shape::constant_shape::Value>(
    data_model::same_shape::constant_shape::Value const& value,
    bool const show_details)
{
    ImGui::Text("nr arrays: ");
    ImGui::SameLine();
    ImGui::Text(fmt::format("{}", value.nr_arrays()).c_str());
    ImGui::SameLine();
    help_marker("For each active object an nD array is stored.");

    ImGui::Text("array shape: ");
    ImGui::SameLine();
    ImGui::Text(shape_as_string(value.array_shape()).c_str());
    ImGui::SameLine();
    help_marker(array_shape_doc);

    show_array(dynamic_cast<data_model::Array const&>(value), show_details);
}


template<>
void show_value<data_model::same_shape::variable_shape::Value>(
    data_model::same_shape::variable_shape::Value const& value,
    bool const show_details)
{
    // TODO(KDJ) nr_locations_in_time

    show_value_group(value, show_details);
    // TODO(KDJ) show_rank(value.rank());
}


template<>
void show_value<data_model::different_shape::Value>(
    data_model::different_shape::Value const& value,
    bool const show_details)
{
    show_nr_objects(value.nr_objects());
    show_value_group(value, show_details);
    // TODO(KDJ) show_rank(value.rank());
}


template<>
void show_value<data_model::different_shape::constant_shape::Value>(
    data_model::different_shape::constant_shape::Value const& value,
    bool const show_details)
{
    show_nr_objects(value.nr_objects());
    show_value_group(value, show_details);
    show_rank(value.rank());
}


template<>
void show_value<data_model::different_shape::variable_shape::Value>(
    data_model::different_shape::variable_shape::Value const& value,
    bool const show_details)
{
    // TODO(KDJ) nr_locations_in_time

    show_value_group(value, show_details);
    // TODO(KDJ) show_rank(value.rank() - 1);
}


void show_object_id(
    data_model::ObjectID const& object_id,
    bool const show_details)
{
    show_value(
        dynamic_cast<data_model::same_shape::Value const&>(object_id),
        show_details);
}


void show_object_tracker(
    data_model::ObjectTracker const& /* object_tracker */,
    bool const /* show_details */)
{
}


std::string epoch_to_string(
    data_model::time::Epoch const& epoch)
{
    std::string result;

    if(!epoch.origin()) {
        result = data_model::aspect_to_string(epoch.kind());
    }
    else {
        if(!epoch.calendar()) {
            result = fmt::format(
                "{} / {}",
                data_model::aspect_to_string(epoch.kind()),
                *epoch.origin());
        }
        else {
            result = fmt::format(
                "{} / {} / {}",
                data_model::aspect_to_string(epoch.kind()),
                *epoch.origin(),
                data_model::aspect_to_string(*epoch.calendar()));
        }
    }

    return result;
}


void show_time_domain(
    data_model::TimeDomain const& domain,
    bool const /* show_details */)
{
    ImGui::Indent();

        // Configuration
        {
            auto const& configuration{domain.configuration()};

            ImGui::Text("item type: ");
            ImGui::SameLine();
            ImGui::Text(data_model::aspect_to_string(
                configuration.value<data_model::TimeDomainItemType>()).c_str());
        }

        // Clock
        {
            auto const& clock{domain.clock()};

            ImGui::Text("epoch: ");
            ImGui::SameLine();
            ImGui::Text(epoch_to_string(clock.epoch()).c_str());

            ImGui::Text("unit: ");
            ImGui::SameLine();
            ImGui::Text(data_model::aspect_to_string(clock.unit()).c_str());

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
    data_model::SpaceDomain const& domain,
    bool const /* show_details */)
{
    ImGui::Indent();

        // Configuration
        {
            auto const& configuration{domain.configuration()};

            ImGui::Text("mobility: ");
            ImGui::SameLine();
            ImGui::Text(data_model::aspect_to_string(
                configuration.value<data_model::Mobility>()).c_str());

            ImGui::Text("item type: ");
            ImGui::SameLine();
            ImGui::Text(data_model::aspect_to_string(
                configuration.value<data_model::SpaceDomainItemType>()).c_str());
        }

        // Discretized presence property
        {
            if(domain.presence_is_discretized()) {
                ImGui::Text("discretized presence property: ");
                ImGui::SameLine();
                ImGui::Text(
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<data_model::SpaceDomain&>(domain)
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
                                        bool show_details);


template<>
void show_property<data_model::same_shape::Property>(
    data_model::same_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<>
void show_property<data_model::same_shape::constant_shape::Property>(
    data_model::same_shape::constant_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<>
void show_property<data_model::same_shape::variable_shape::Property>(
    data_model::same_shape::variable_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<>
void show_property<data_model::different_shape::Property>(
    data_model::different_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<>
void show_property<data_model::different_shape::constant_shape::Property>(
    data_model::different_shape::constant_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<>
void show_property<data_model::different_shape::variable_shape::Property>(
    data_model::different_shape::variable_shape::Property const& property,
    bool const show_details)
{
    show_value(property.value(), show_details);
}


template<
    typename Collection>
void show_properties(
    data_model::Properties const& properties,
    bool const show_details)
{
    Collection const& collection{properties.collection<Collection>()};

    if(!collection.empty()) {

        ImGui::Indent();

            ImGui::BeginGroup();

            ImGui::Text("shape per object: ");
            ImGui::SameLine();
            ImGui::Text(data_model::aspect_to_string(data_model::shape_per_object<Collection>).c_str());

            ImGui::Text("value variability: ");
            ImGui::SameLine();
            ImGui::Text(data_model::aspect_to_string(data_model::value_variability<Collection>).c_str());

            ImGui::Text("shape variability: ");
            ImGui::SameLine();
            ImGui::Text(data_model::aspect_to_string(data_model::shape_variability<Collection>).c_str());

            for(std::string const& name: collection.names())
            {
                if(auto tree_node = gui::TreeNode(name, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    copy_popup("property name", name);
                    show_property(collection[name], show_details);
                }
            }

            ImGui::Separator();
            ImGui::EndGroup();

        ImGui::Unindent();
    }
}


template<
    typename Collection>
void show_properties2(
    data_model::Properties const& properties,
    bool const show_details)
{
    Collection const& collection{properties.collection<Collection>()};

    for(std::string const& property_name: collection.names())
    {
        if(auto tab_item = gui::TabItem(property_name))
        {
            ImGui::Indent();

                // ImGui::BeginGroup();

                if(show_details) {
                    ImGui::Text("shape per object: ");
                    ImGui::SameLine();
                    ImGui::Text(data_model::aspect_to_string(data_model::shape_per_object<Collection>).c_str());

                    ImGui::Text("value variability: ");
                    ImGui::SameLine();
                    ImGui::Text(data_model::aspect_to_string(data_model::value_variability<Collection>).c_str());

                    ImGui::Text("shape variability: ");
                    ImGui::SameLine();
                    ImGui::Text(data_model::aspect_to_string(data_model::shape_variability<Collection>).c_str());
                }

                if(!collection[property_name].description().empty()) {
                    ImGui::TextWrapped(collection[property_name].description().c_str());
                }
                show_property(collection[property_name], show_details);

                // ImGui::Separator();
                // ImGui::EndGroup();

            ImGui::Unindent();
        }
    }
}


void show_property_set(
    data_model::PropertySet const& property_set,
    bool const show_details)
{
    if(property_set.has_time_domain()) {
        if(auto tree_node = gui::TreeNode("object tracker"))
        {
            show_object_tracker(property_set.object_tracker(), show_details);
        }

        if(auto tree_node = gui::TreeNode("time domain", ImGuiTreeNodeFlags_DefaultOpen))
        {
            show_time_domain(property_set.time_domain(), show_details);
        }
    }

    if(property_set.has_space_domain()) {
        if(auto tree_node = gui::TreeNode("space domain", ImGuiTreeNodeFlags_DefaultOpen))
        {
            show_space_domain(property_set.space_domain(), show_details);
        }
    }

    {
        auto const& properties = property_set.properties();

        if(auto tree_node = gui::TreeNode(
            fmt::format("properties ({})", properties.size()),
            ImGuiTreeNodeFlags_DefaultOpen))
        {

            // show_properties<same_shape::Properties>(
            //     properties, show_details);
            // show_properties<same_shape::constant_shape::Properties>(
            //     properties, show_details);
            // show_properties<same_shape::variable_shape::Properties>(
            //     properties, show_details);

            // show_properties<different_shape::Properties>(
            //     properties, show_details);
            // show_properties<different_shape::constant_shape::Properties>(
            //     properties, show_details);
            // show_properties<different_shape::variable_shape::Properties>(
            //     properties, show_details);



            if(auto tab_bar = gui::TabBar("Properties"))
            {
                show_properties2<data_model::same_shape::Properties>(
                    properties, show_details);
                show_properties2<data_model::same_shape::constant_shape::Properties>(
                    properties, show_details);
                show_properties2<data_model::same_shape::variable_shape::Properties>(
                    properties, show_details);

                show_properties2<data_model::different_shape::Properties>(
                    properties, show_details);
                show_properties2<data_model::different_shape::constant_shape::Properties>(
                    properties, show_details);
                show_properties2<data_model::different_shape::variable_shape::Properties>(
                    properties, show_details);
            }


            // if(!properties.empty()) {

            //     auto const names{properties.names()};

            //     if(ImGui::BeginCombo("properties", names.front().c_str())) {



            //         // for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            //         // {
            //         //     bool is_selected = (item_current == items[n]);
            //         //     if (ImGui::Selectable(items[n], is_selected))
            //         //         item_current = items[n];
            //         //     if (is_selected)
            //         //         ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            //         // }

            //         ImGui::EndCombo();
            //     }
            // }

        }
    }
}


void show_property_sets(
    data_model::PropertySets const& property_sets,
    bool const show_details)
{
    if(auto tab_bar = gui::TabBar("Property-sets"))
    {
        for(std::string const& name: property_sets.names())
        {
            if(auto tab_item = gui::TabItem(name))
            {
                copy_popup("property-set name", name);
                show_property_set(property_sets[name], show_details);
            }
        }
    }

    // for(std::string const& name: property_sets.names()) {
    //     if(ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

    //         copy_popup("property-set name", name);
    //         show_property_set(property_sets[name], show_details);
    //         ImGui::TreePop();

    //     }
    // }
}


void show_phenomenon(
    data_model::Phenomenon const& phenomenon,
    bool const show_details)
{
    if(!phenomenon.description().empty()) {
        ImGui::TextWrapped(phenomenon.description().c_str());
    }

    {
        auto const& object_id{phenomenon.object_id()};

        if(show_details || object_id.nr_objects() > 0) {
            if(auto tree_node = gui::TreeNode(
                fmt::format("object_id ({})", object_id.nr_objects())))
            {
                show_object_id(object_id, show_details);
            }
        }
    }

    {
        auto const& property_sets{phenomenon.collection_property_sets()};

        if(show_details || !property_sets.empty()) {
            if(auto tree_node = gui::TreeNode(
                fmt::format("collection property-sets ({})", property_sets.size()),
                ImGuiTreeNodeFlags_DefaultOpen))
            {
                show_property_sets(property_sets, show_details);
            }
        }
    }

    {
        auto const& property_sets{phenomenon.property_sets()};

        if(show_details || !property_sets.empty()) {
            if(auto tree_node = gui::TreeNode(
                fmt::format("property-sets ({})", property_sets.size()),
                ImGuiTreeNodeFlags_DefaultOpen))
            {
                show_property_sets(property_sets, show_details);
            }
        }
    }
}


void show_phenomena(
    data_model::Phenomena const& phenomena,
    bool const show_details)
{
    if(auto tab_bar = gui::TabBar("Phenomena"))
    {
        for(std::string const& name: phenomena.names())
        {
            if(auto tab_item = gui::TabItem(name))
            {
                copy_popup("phenomenon name", name);
                show_phenomenon(phenomena[name], show_details);
            }
        }
    }

    // for(std::string const& name: phenomena.names()) {

    //     if(ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

    //         copy_popup("phenomenon name", name);
    //         show_phenomenon(phenomena[name], show_details);
    //         ImGui::TreePop();

    //     }
    // }
}


void show_universe(
    data_model::Universe const& universe,
    bool const show_details)
{
    if(auto tree_node = gui::TreeNode("phenomena", ImGuiTreeNodeFlags_DefaultOpen))
    {
        show_phenomena(universe.phenomena(), show_details);
    }
}


void show_universes(
    data_model::Universes const& universes,
    bool const show_details)
{
    if(auto tab_bar = gui::TabBar("Universes"))
    {
        for(std::string const& name: universes.names())
        {
            if(auto tab_item = gui::TabItem(name))
            {
                copy_popup("property-set name", name);
                show_universe(universes[name], show_details);
            }
        }
    }

    // for(std::string const& name: universes.names()) {
    //     if(ImGui::TreeNodeEx(name.c_str())) {

    //         copy_popup("universe name", name);
    //         show_universe(universes[name], show_details);
    //         ImGui::TreePop();

    //     }
    // }
}


void show_dataset(
    Dataset const& dataset,
    bool const show_details)
{
    lue::data_model::Dataset const& source{dataset.dataset()};

    ImGui::Indent();

        if(!source.description().empty()) {
            ImGui::TextWrapped(source.description().c_str());
        }

        if(show_details) {

            ImGui::Text("LUE version: ");
            ImGui::SameLine();
            ImGui::Text(source.lue_version().c_str());
            ImGui::SameLine();
            help_marker("The version of LUE used to create the dataset");

            show_file(dynamic_cast<hdf5::File const&>(source));
        }

    ImGui::Unindent();

    {
        auto const& phenomena{source.phenomena()};

        if(show_details || !phenomena.empty())
        {
            if(auto tree_node = gui::TreeNode(
                fmt::format("phenomena ({})", phenomena.size()),
                ImGuiTreeNodeFlags_DefaultOpen))
            {
                show_phenomena(phenomena, show_details);
            }
        }
    }

    {
        auto const& universes{source.universes()};

        if(show_details || !universes.empty())
        {
            if(auto tree_node = gui::TreeNode(
                fmt::format("universes ({})", universes.size()),
                ImGuiTreeNodeFlags_DefaultOpen))
            {
                show_universes(universes, show_details);
            }
        }
    }
}


void show_datasets(
    Datasets& datasets,
    bool const show_details)
{
    // Window for presenting information about the loaded datasets
    sdl2::imgui::Window imgui_window{"Datasets"};

    if(auto tab_bar = gui::TabBar("Datasets"))
    {
        for(auto& dataset: datasets)
        {
            if(auto tab_item = gui::TabItem(dataset.filename()))
            {
                copy_popup("filename", dataset.filename());
                dataset.rescan();

                if(dataset.is_open()) {
                    show_dataset(dataset, show_details);
                }
            }
        }
    }
}

}  // Anonymous namespace


Application::Application(
    std::vector<std::string> const& arguments)

    : utility::Application{usage, arguments}

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

    sdl2::API api;
    sdl2::Window sdl_window{"LUE view",
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800};
    sdl2::ImGuiBinding binding{api, sdl_window};

    // static std::string const ini_pathname{
    //     expand_environment_variables("${HOME}/.lue_view.ini")};
    // binding.io().IniFilename = ini_pathname.c_str();
    sdl2::ImGuiBinding::io().IniFilename = nullptr;  // Skip for now

    // Open datasets
    Datasets datasets_to_visualize{dataset_names.begin(), dataset_names.end()};

    bool stop_browsing = false;

    while(!stop_browsing) {

        // Handle key presses
        SDL_Event event;
        while(SDL_PollEvent(&event) != 0) {

            ImGui_ImplSDL2_ProcessEvent(&event);

            if(
                event.type == SDL_QUIT || (
                event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(sdl_window)))
            {
                stop_browsing = true;
            }
        }

        // Draw stuff on window
        // TODO(KDJ)

        sdl2::imgui::Frame frame{sdl_window};

        static Configuration configuration{};
        show_main_menu_bar(configuration);
        show_datasets(datasets_to_visualize, configuration.show_details());
    }

    return EXIT_SUCCESS;
}

}  // namespace view
}  // namespace lue
