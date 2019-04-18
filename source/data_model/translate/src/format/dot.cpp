#include "lue/translate/format/dot.hpp"
#include <fmt/format.h>
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/transformed.hpp>


// Unused suitable color: "fillcolor": "#094d8e"


namespace lue {
namespace utility {

using Record = std::tuple<std::string, std::string>;
using Table = std::vector<Record>;

std::string to_dot(
    Record const& record)
{
    auto cell1 = std::get<0>(record);
    auto cell2 = std::get<1>(record);

    return cell2.empty()
        ?
            "<tr>"
                "<td align=\"center\" colspan=\"2\">" + cell1 + "</td>"
            "</tr>"

        :
            "<tr>"
                "<td align=\"left\">" + cell1 + "</td>"
                "<td align=\"left\">" + cell2 + "</td>"
            "</tr>"
        ;
}


std::string to_dot(
    Table const& table)
{
    std::string result = "<table"
       " border=\"0\""
    ">";

    for(auto const& record: table) {
        result += to_dot(record);
    }

    result += "</table>";

    return result;
}


std::string to_string(
    hdf5::Shape const& shape)
{
    auto to_string = [](hdf5::Shape::value_type const value) {
        return std::to_string(value);
    };

    return
        "(" +
        boost::join(shape | boost::adaptors::transformed(to_string), ", ") +
        ")"
        ;
}


std::string dot_name(
    hdf5::Identifier const& id)
{
    return "node_" + std::to_string(id.info().addr());
}


std::string dot_name(
    hdf5::PrimaryDataObject const& data_object)
{
    return dot_name(data_object.id());
}


// Shapes are based on the HDF5 object type ------------------------------------
std::string shape(
    hdf5::Group const&,
    Metadata const& metadata)
{
    return metadata.string(JSONPointer("/hdf5/group/shape"), "oval");
}


std::string shape(
    hdf5::Dataset const&,
    Metadata const& metadata)
{
    return metadata.string(JSONPointer("/hdf5/dataset/shape"), "box");
}


// Colors are based on the LUE type --------------------------------------------
std::string default_fillcolor()
{
    return "transparent";
}


template<
    typename T>
std::string fillcolor(
    T const&)
{
    return default_fillcolor();
}


class Subgraph
{

public:

    Subgraph(
        std::ostream& stream,
        hdf5::Identifier const id,
        std::string const& label)
    :
        _stream{stream}

    {
        _stream << fmt::format(R"(
subgraph cluster_{} {{
    label=<{}>
)",
            dot_name(id),
            label
            );
    }

    Subgraph(
        std::ostream& stream,
        hdf5::Identifier const id)
    :
        Subgraph{stream, id, id.name()}

        {
        }

    ~Subgraph()
    {
        _stream << "}\n\n";
    }

private:

    std::ostream& _stream;

};


template<
    typename T1,
    typename T2>
void link_nodes(
    T1 const& from_object,
    T2 const& to_object,
    std::ostream& stream,
    Metadata const& /* metadata */,
    int weight=10,
    std::string const& style="solid",
    std::string const& label="")
{
    stream << fmt::format(R"(
    {} -> {} [
        weight={}
        style="{}"
        label=<{}>
    ];
)",
        dot_name(from_object),
        dot_name(to_object),
        weight,
        style,
        label);
}


template<
    typename T1,
    typename T2>
void soft_link_nodes(
    T1 const& from_object,
    T2 const& to_object,
    std::ostream& stream,
    Metadata const& metadata,
    std::string const& label="")
{
    link_nodes(
        from_object, to_object, stream, metadata, 0, "dashed", label);
}


template<
    typename Object>
static void mark_as_empty(
    Object const& object,
    std::ostream& stream)
{
    // Dump something to notify the user that this part of the dataset
    // is empty
    stream << fmt::format(R"(
    {} [
        label="<empty>"
        shape="plaintext"
        fillcolor="red"
    ];
)",
        dot_name(object))
        // fillcolor(object))
        ;
}

}  // namespace utility


using namespace utility;


// We are in the lue namespace now. This make the code below easier to write.
// We need less explicit namespace qualifications.

void to_dot(
    same_shape::Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    std::string const label = metadata.boolean(
            JSONPointer("/lue/value/show_details"), false)
        ? to_dot(Table{
                Record{value.id().name(), ""},
                Record{"shape_per_object", "same"},
                Record{"nr_arrays", std::to_string(value.nr_arrays())},
                Record{"array_shape", to_string(value.array_shape())},
                Record{"file_datatype",
                    hdf5::standard_datatype_as_string(
                        (value.file_datatype()))},
            })
        : value.id().name()
        ;

    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(value),
        label,
        shape(value, metadata),
        metadata.string(
            JSONPointer("/lue/value/fillcolor"), fillcolor(value)));
}


void to_dot(
    different_shape::Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    std::string const label = metadata.boolean(
            JSONPointer("/lue/value/show_details"), false)
        ? to_dot(Table{
                Record{value.id().name(), ""},
                Record{"shape_per_object", "different"},
                Record{"rank", std::to_string(value.rank())},
                Record{"file_datatype",
                    hdf5::standard_datatype_as_string(
                        (value.file_datatype()))},
            })
        : value.id().name()
        ;

    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(value),
        label,
        shape(value, metadata),
        metadata.string(
            JSONPointer("/lue/value/fillcolor"), fillcolor(value)));
}


void to_dot(
    same_shape::constant_shape::Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    std::string const label = metadata.boolean(
            JSONPointer("/lue/value/show_details"), false)
        ? to_dot(Table{
                Record{value.id().name(), ""},
                Record{"shape_per_object", "same"},
                Record{"shape_variability", "constant"},
                Record{"nr_arrays", std::to_string(value.nr_arrays())},
                Record{"array_shape", to_string(value.array_shape())},
                Record{"file_datatype",
                    hdf5::standard_datatype_as_string(
                        (value.file_datatype()))},
            })
        : value.id().name()
        ;

    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(value),
        label,
        shape(value, metadata),
        metadata.string(
            JSONPointer("/lue/value/fillcolor"), fillcolor(value)));
}


void to_dot(
    different_shape::constant_shape::Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    std::string const label = metadata.boolean(
            JSONPointer("/lue/value/show_details"), false)
        ? to_dot(Table{
                Record{value.id().name(), ""},
                Record{"shape_per_object", "different"},
                Record{"shape_variability", "constant"},
                Record{"nr_objects", std::to_string(value.nr_objects())},
                Record{"rank", std::to_string(value.rank())},
                Record{"file_datatype",
                    hdf5::standard_datatype_as_string(
                        (value.file_datatype()))},
            })
        : value.id().name()
        ;

    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(value),
        label,
        shape(value, metadata),
        metadata.string(
            JSONPointer("/lue/value/fillcolor"), fillcolor(value)));
}


void to_dot(
    same_shape::variable_shape::Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    std::string const label = metadata.boolean(
            JSONPointer("/lue/value/show_details"), false)
        ? to_dot(Table{
                Record{value.id().name(), ""},
                Record{"shape_per_object", "same"},
                Record{"shape_variability", "variable"},
                Record{"nr_locations_in_time",
                    std::to_string(value.nr_locations_in_time())},
                Record{"rank", std::to_string(value.rank())},
                Record{"file_datatype",
                    hdf5::standard_datatype_as_string(
                        (value.file_datatype()))},
            })
        : value.id().name()
        ;

    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(value),
        label,
        shape(value, metadata),
        metadata.string(
            JSONPointer("/lue/value/fillcolor"), fillcolor(value)));
}


void to_dot(
    different_shape::variable_shape::Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    std::string const label = metadata.boolean(
            JSONPointer("/lue/value/show_details"), false)
        ? to_dot(Table{
                Record{value.id().name(), ""},
                Record{"shape_per_object", "different"},
                Record{"shape_variability", "variable"},
                Record{"nr_locations_in_time",
                    std::to_string(value.nr_locations_in_time())},
                Record{"rank", std::to_string(value.rank())},
                Record{"file_datatype",
                    hdf5::standard_datatype_as_string(
                        (value.file_datatype()))},
            })
        : value.id().name()
        ;

    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(value),
        label,
        shape(value, metadata),
        metadata.string(
            JSONPointer("/lue/value/fillcolor"), fillcolor(value)));
}


void to_dot(
    TimeCell const& time_cell,
    std::ostream& stream,
    Metadata const& metadata)
{
    to_dot(dynamic_cast<TimeBox const&>(time_cell), stream, metadata);

    // Also dump the counts and link the boxes to the counts
    to_dot(time_cell.count(), stream, metadata);
    link_nodes(time_cell, time_cell.count(), stream, metadata);
}


void to_dot(
    PropertyGroup& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(property),
        property.id().name(),
        shape(property, metadata),
        metadata.string(
            JSONPointer("/lue/property/fillcolor"),
            fillcolor(property)));

    if(property.time_is_discretized()) {
        auto const show_details =
            metadata.boolean(JSONPointer("/lue/time_domain/show_details"),
            false);
        soft_link_nodes(property, property.time_discretization_property(),
            stream, metadata,
            show_details
                ? aspect_to_string(property.time_discretization_type())
                : "");
    }

    if(property.space_is_discretized()) {
        auto const show_details =
            metadata.boolean(JSONPointer("/lue/space_domain/show_details"),
            false);
        soft_link_nodes(property, property.space_discretization_property(),
            stream, metadata,
            show_details
                ? aspect_to_string(property.space_discretization_type())
                : "");
    }
}


void to_dot(
    same_shape::Property& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    to_dot(dynamic_cast<PropertyGroup&>(property), stream, metadata);

    if(metadata.boolean(JSONPointer("/lue/value/show"), false)) {
        to_dot(property.value(), stream, metadata);
        link_nodes(property, property.value(), stream, metadata);
    }
}


void to_dot(
    different_shape::Property& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    to_dot(dynamic_cast<PropertyGroup&>(property), stream, metadata);

    if(metadata.boolean(JSONPointer("/lue/value/show"), false)) {
        to_dot(property.value(), stream, metadata);
        link_nodes(property, property.value(), stream, metadata);
    }
}


void to_dot(
    same_shape::constant_shape::Property& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    to_dot(dynamic_cast<PropertyGroup&>(property), stream, metadata);

    if(metadata.boolean(JSONPointer("/lue/value/show"), false)) {
        to_dot(property.value(), stream, metadata);
        link_nodes(property, property.value(), stream, metadata);
    }
}


void to_dot(
    different_shape::constant_shape::Property& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    to_dot(dynamic_cast<PropertyGroup&>(property), stream, metadata);

    if(metadata.boolean(JSONPointer("/lue/value/show"), false)) {
        to_dot(property.value(), stream, metadata);
        link_nodes(property, property.value(), stream, metadata);
    }
}


void to_dot(
    same_shape::variable_shape::Property& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    to_dot(dynamic_cast<PropertyGroup&>(property), stream, metadata);

    if(metadata.boolean(JSONPointer("/lue/value/show"), false)) {
        to_dot(property.value(), stream, metadata);
        link_nodes(property, property.value(), stream, metadata);
    }
}


void to_dot(
    different_shape::variable_shape::Property& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    to_dot(dynamic_cast<PropertyGroup&>(property), stream, metadata);

    if(metadata.boolean(JSONPointer("/lue/value/show"), false)) {
        to_dot(property.value(), stream, metadata);
        link_nodes(property, property.value(), stream, metadata);
    }
}


void to_dot(
    TimeDomain& time_domain,
    std::ostream& stream,
    Metadata const& metadata)
{
    auto const configuration = time_domain.configuration();
    auto const item_type = configuration.value<TimeDomainItemType>();
    auto const& clock = time_domain.clock();

    std::string const label = metadata.boolean(
            JSONPointer("/lue/time_domain/show_details"), false)
        ? to_dot(Table{
                Record{time_domain.id().name(), ""},
                Record{"unit", aspect_to_string(clock.unit())},
                Record{"nr_units", std::to_string(clock.nr_units())},
                Record{"item_type", aspect_to_string(item_type)},
            })
        : time_domain.id().name()
        ;

    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(time_domain),
        label,
        shape(time_domain, metadata),
        metadata.string(
            JSONPointer("/lue/time_domain/fillcolor"),
            fillcolor(time_domain)));


    if(metadata.boolean(JSONPointer("/lue/value/show"), false)) {
        switch(configuration.value<TimeDomainItemType>()) {
            case TimeDomainItemType::box: {
                auto const& value = time_domain.value<TimeBox>();
                to_dot(value, stream, metadata);
                link_nodes(time_domain, value, stream, metadata);
                break;
            }
            case TimeDomainItemType::cell: {
                auto const& value = time_domain.value<TimeCell>();
                to_dot(value, stream, metadata);
                link_nodes(time_domain, value, stream, metadata);
                break;
            }
            case TimeDomainItemType::point: {
                auto const& value = time_domain.value<TimePoint>();
                to_dot(value, stream, metadata);
                link_nodes(time_domain, value, stream, metadata);
                break;
            }
        }
    }
}


void to_dot(
    SpaceDomain& space_domain,
    std::ostream& stream,
    Metadata const& metadata)
{
    auto const configuration = space_domain.configuration();
    auto const mobility = configuration.value<Mobility>();
    auto const item_type = configuration.value<SpaceDomainItemType>();

    std::string const label = metadata.boolean(
            JSONPointer("/lue/space_domain/show_details"), false)
        ? to_dot(Table{
                Record{space_domain.id().name(), ""},
                Record{"mobility", aspect_to_string(mobility)},
                Record{"item_type", aspect_to_string(item_type)},
            })
        : space_domain.id().name()
        ;

    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(space_domain),
        label,
        shape(space_domain, metadata),
        metadata.string(
            JSONPointer("/lue/space_domain/fillcolor"),
            fillcolor(space_domain)));

    if(metadata.boolean(JSONPointer("/lue/value/show"), false)) {
        switch(mobility) {
            case Mobility::mobile: {
                switch(item_type) {
                    case SpaceDomainItemType::box: {
                        auto const& value = space_domain.value<MobileSpaceBox>();
                        to_dot(value, stream, metadata);
                        link_nodes(space_domain, value, stream, metadata);
                        break;
                    }
                    case SpaceDomainItemType::point: {
                        auto const& value = space_domain.value<MobileSpacePoint>();
                        to_dot(value, stream, metadata);
                        link_nodes(space_domain, value, stream, metadata);
                        break;
                    }
                }

                break;
            }
            case Mobility::stationary: {
                switch(item_type) {
                    case SpaceDomainItemType::box: {
                        auto const& value =
                            space_domain.value<StationarySpaceBox>();
                        to_dot(value, stream, metadata);
                        link_nodes(space_domain, value, stream, metadata);
                        break;
                    }
                    case SpaceDomainItemType::point: {
                        auto const& value =
                            space_domain.value<StationarySpacePoint>();
                        to_dot(value, stream, metadata);
                        link_nodes(space_domain, value, stream, metadata);
                        break;
                    }
                }

                break;
            }
        }
    }
}


void to_dot(
    ObjectTracker const& object_tracker,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(object_tracker),
        object_tracker.id().name(),
        shape(object_tracker, metadata),
        metadata.string(
            JSONPointer("/lue/object_tracker/fillcolor"),
            fillcolor(object_tracker)));

    if(metadata.boolean(JSONPointer("/lue/value/show"), false)) {
        {
            auto const& active_set_index = object_tracker.active_set_index();
            to_dot(active_set_index, stream, metadata);
            link_nodes(object_tracker, active_set_index, stream, metadata);
        }

        {
            auto const& active_object_index =
                object_tracker.active_object_index();
            to_dot(active_object_index, stream, metadata);
            link_nodes(object_tracker, active_object_index, stream, metadata);
        }

        {
            auto const& active_object_id = object_tracker.active_object_id();
            to_dot(active_object_id, stream, metadata);
            link_nodes(object_tracker, active_object_id, stream, metadata);
        }
    }
}


/*!
    @brief      Translate the structure of a property-set to a DOT graph
*/
void to_dot(
    PropertySet& property_set,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(property_set),
        property_set.id().name(),
        shape(property_set, metadata),
        metadata.string(
            JSONPointer("/lue/property_set/fillcolor"),
            fillcolor(property_set)));

    if(metadata.boolean(JSONPointer("/lue/object_tracker/show"), false)) {
        auto const& object_tracker = property_set.object_tracker();

        if(property_set.owns_object_tracker()) {
            to_dot(object_tracker, stream, metadata);
            link_nodes(property_set, object_tracker, stream, metadata);
        }
        else {
            soft_link_nodes(property_set, object_tracker, stream, metadata);
        }
    }

    if(property_set.has_time_domain()) {
        auto& time_domain = property_set.time_domain();

        if(property_set.owns_time_domain()) {
            to_dot(time_domain, stream, metadata);
            link_nodes(property_set, time_domain, stream, metadata);
        }
        else {
            soft_link_nodes(property_set, time_domain, stream, metadata);
        }
    }

    if(property_set.has_space_domain()) {
        auto& space_domain = property_set.space_domain();
        to_dot(space_domain, stream, metadata);
        link_nodes(property_set, space_domain, stream, metadata);

        if(space_domain.presence_is_discretized()) {
            auto const& discretized_presence =
                space_domain.discretized_presence_property();
            soft_link_nodes(
                space_domain, discretized_presence, stream, metadata);
        }
    }

    auto& properties = property_set.properties();

    for(auto& property:
            properties.collection<same_shape::Properties>()) {
        to_dot(property.second, stream, metadata);
        link_nodes(property_set, property.second, stream, metadata, 0);
    }

    for(auto& property: properties.collection<
            different_shape::Properties>()) {
        to_dot(property.second, stream, metadata);
        link_nodes(property_set, property.second, stream, metadata, 0);
    }

    for(auto& property: properties.collection<
            same_shape::constant_shape::Properties>()) {
        to_dot(property.second, stream, metadata);
        link_nodes(property_set, property.second, stream, metadata, 0);
    }

    for(auto& property: properties.collection<
            different_shape::constant_shape::Properties>()) {
        to_dot(property.second, stream, metadata);
        link_nodes(property_set, property.second, stream, metadata, 0);
    }

    for(auto& property: properties.collection<
            same_shape::variable_shape::Properties>()) {
        to_dot(property.second, stream, metadata);
        link_nodes(property_set, property.second, stream, metadata, 0);
    }

    for(auto& property: properties.collection<
            different_shape::variable_shape::Properties>()) {
        to_dot(property.second, stream, metadata);
        link_nodes(property_set, property.second, stream, metadata, 0);
    }
}


/*!
    @brief      Translate the structure of a phenomenon to a DOT graph
*/
void to_dot(
    Phenomenon& phenomenon,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << fmt::format(R"(
    {} [
        label=<{}>
        shape="{}"
        fillcolor="{}"
    ];
)",
        dot_name(phenomenon),
        phenomenon.id().name(),
        shape(phenomenon, metadata),
        metadata.string(
            JSONPointer("/lue/phenomenon/fillcolor"), fillcolor(phenomenon))
    );

    if(metadata.boolean(JSONPointer("/lue/object_tracker/show"), false)) {
        if(metadata.boolean(JSONPointer("/lue/value/show"), false)) {
            auto const& object_id = phenomenon.object_id();
            to_dot(object_id, stream, metadata);
            link_nodes(phenomenon, object_id, stream, metadata);
        }
    }

    auto& property_sets = phenomenon.property_sets();

    for(auto const& name: property_sets.names()) {
        auto& property_set = property_sets[name];
        to_dot(property_set, stream, metadata);
        link_nodes(phenomenon, property_set, stream, metadata, 0);
    }


    auto& collection_property_sets = phenomenon.collection_property_sets();

    for(auto const& name: collection_property_sets.names()) {
        auto& property_set = collection_property_sets[name];
        to_dot(property_set, stream, metadata);
        link_nodes(phenomenon, property_set, stream, metadata, 0);
    }
}


/*!
    @brief      Translate the structure of a universe to a DOT graph
*/
void to_dot(
    Universe& universe,
    std::ostream& stream,
    Metadata const& metadata)
{
    Subgraph graph(stream, universe.id(), universe.id().name());

    auto& phenomena = universe.phenomena();

    for(auto const& name: phenomena.names()) {
        to_dot(phenomena[name], stream, metadata);
    }

    if(phenomena.empty()) {
        mark_as_empty(universe, stream);
    }
}


/*!
    @brief      Translate the structure of a LUE dataset to a DOT graph

    Each dataset ends up in a sub-graph, to set it apart from other
    datasets.
*/
void to_dot(
    Dataset& dataset,
    std::ostream& stream,
    Metadata const& metadata)
{
    Subgraph graph(stream, dataset.id(), dataset.pathname());

    auto& universes = dataset.universes();

    for(auto const& name: universes.names()) {
        to_dot(universes[name], stream, metadata);
    }

    auto& phenomena = dataset.phenomena();

    for(auto const& name: phenomena.names()) {
        to_dot(phenomena[name], stream, metadata);
    }


    if(universes.empty() && phenomena.empty()) {
        // In this case, the subgraph won't be visualized. Just dump
        // something to signal the user that the dataset is empty.
        mark_as_empty(dataset, stream);
    }
}


/*!
    @brief      Translate the structure of multiple LUE datasets to a
                DOT graph
*/
void to_dot(
    DatasetRefs const& datasets,
    std::ostream& stream,
    Metadata const& metadata)
{
    if(datasets.empty()) {
        return;
    }

    // Create a DOT graph representation of the dataset. We don't have to
    // write the data values. We do have to write the organization, in HDF5,
    // of the data values.

    stream << fmt::format(R"(digraph {{
    fontname="Courier"
    fontsize=10

    graph [
        bgcolor="transparent"
    ];

    node [
        fontname="Courier"
        fontsize=10
        fontcolor="{}"
        style="filled"
    ];

    edge [
        fontname="Courier"
        fontsize=10
        arrowhead="open"
        arrowtail="open"
    ];

)",
        metadata.string(
            JSONPointer("/dot/node/fontcolor"), "black")
    );

    for(auto& dataset: datasets) {
        to_dot(dataset, stream, metadata);
    }

    stream << R"(}
)";
}


namespace utility {

/*!
    @brief      Translate the structure of multiple LUE datasets to a
                DOT graph
*/
void translate_lue_datasets_to_dot(
    DatasetRefs const& datasets,
    std::string const& dot_filename,
    Metadata const& metadata)
{
    std::ofstream stream(dot_filename);

    if(!stream) {
        throw std::runtime_error("cannot open file " + dot_filename);
    }

    to_dot(datasets, stream, metadata);
}


/*!
    @brief      Translate the structure of a LUE dataset to a DOT graph
*/
void translate_lue_dataset_to_dot(
    Dataset& dataset,
    std::ostream& stream,
    Metadata const& metadata)
{
    DatasetRefs dataset_ref{dataset};
    to_dot(dataset_ref, stream, metadata);
}


/*!
    @brief      Translate the structure of a LUE dataset to a DOT graph
*/
void translate_lue_dataset_to_dot(
    Dataset& dataset,
    std::string const& dot_filename,
    Metadata const& metadata)
{
    DatasetRefs dataset_ref{dataset};
    translate_lue_datasets_to_dot(dataset_ref, dot_filename, metadata);
}

}  // namespace utility
}  // namespace lue
