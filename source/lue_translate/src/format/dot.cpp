#include "lue/translate/format/dot.hpp"
#include "lue/constant_size/time/located/property_set.hpp"
#include "lue/constant_size/time/located/shared/constant_shape/property.hpp"
#include "lue/constant_size/time/located/shared/time_box_domain.hpp"
#include "lue/constant_size/time/omnipresent/different_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"
#include "lue/space_domain.hpp"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <exception>
#include <fstream>
#include <functional>


namespace lue {
namespace utility {

using Record = std::tuple<std::string, std::string>;
using Table = std::vector<Record>;


std::string to_dot(
    Record const& record)
{
    return
        "<tr>"
            "<td align=\"left\">" +
                std::get<0>(record) +
            "</td>"
            "<td align=\"left\">" +
                std::get<1>(record) +
            "</td>"
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
        "[" +
        boost::join(shape | boost::adaptors::transformed(to_string), ", ") +
        "]"
        ;
}


std::string dot_name(
    hdf5::Identifier const& id)
{
    return "node_" + std::to_string(id.info().addr());
}


std::string dot_name(
    hdf5::Dataset const& dataset)
{
    return dot_name(dataset.id());
}


std::string dot_name(
    hdf5::Group const& group)
{
    return dot_name(group.id());
}


// Shapes are based on the HDF5 type -------------------------------------------
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

        : _stream(stream)

    {
        _stream << boost::str(boost::format(R"(
subgraph cluster_%1% {
    label=<%2%>
)")
            % dot_name(id)
            % label
            );
    }

    Subgraph(
        std::ostream& stream,
        hdf5::Identifier const id)

        : Subgraph(stream, id, id.name())

        {
        }

    ~Subgraph()
    {
        _stream << "}\n\n";
    }

private:

    std::ostream& _stream;

};


void dump_node(
    Phenomenon const& phenomenon,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        shape="%3%"
        fillcolor="%4%"
    ];
)")
        % dot_name(phenomenon)
        % phenomenon.id().name()
        % shape(phenomenon, metadata)
        % metadata.string(
            JSONPointer("/lue/phenomenon/fillcolor"), fillcolor(phenomenon))
    );
}


void dump_node(
    Domain const& domain,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        shape="%3%"
        fillcolor="%4%"
    ];
)")
        % dot_name(domain)
        % domain.id().name()
        % shape(domain, metadata)
        % metadata.string(
            JSONPointer("/lue/domain/fillcolor"), fillcolor(domain))
    );
}


void dump_node(
    SpaceDomain const& space_domain,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        shape="%3%"
        fillcolor="%4%"
    ];
)")
        % dot_name(space_domain)
        % space_domain.id().name()
        % shape(space_domain, metadata)
        % metadata.string(
            JSONPointer("/lue/space_domain/fillcolor"),
            fillcolor(space_domain))
    );
}


template<
    typename T1,
    typename T2>
void link_nodes(
    T1 const& from_object,
    T2 const& to_object,
    std::ostream& stream,
    Metadata const& /* metadata */,
    double weight=1.0,
    std::string const& style="solid")
{
    stream << boost::str(boost::format(R"(
    %1% -> %2% [
        weight=%3%
        style="%4%"
    ];
)")
        % dot_name(from_object)
        % dot_name(to_object)
        % weight
        % style
        );
}

}  // namespace utility


using namespace utility;


// We are in the lue namespace now. This make the code below easier to write.
// We need less explicit namespace qualifications.

void to_dot(
    Domain const& domain,
    std::ostream& stream,
    Metadata const& metadata)
{
    dump_node(domain, stream, metadata);

    if(space_domain_exists(domain)) {
        auto const space_domain = SpaceDomain(domain);

        dump_node(space_domain, stream, metadata);
        link_nodes(domain, space_domain, stream, metadata);
    }
}


namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {

void dump_node(
    Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    std::string const label = metadata.boolean(
            JSONPointer("/lue/value/show_details"), false)
        ? to_dot(Table{
                Record{ value.id().name(), "" },
                Record{ "nr_items", std::to_string(value.nr_items()) },
                Record{ "shape", to_string(value.value_shape()) },
                Record{ "file_datatype",
                    hdf5::standard_datatype_as_string(
                        (value.file_datatype())) },
                Record{ "memory_datatype",
                    hdf5::native_datatype_as_string(
                        (value.memory_datatype())) }
            })
        : value.id().name()
        ;

    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        shape="%3%"
        fillcolor="%4%"
    ];
)")
        % dot_name(value)
        % label
        % shape(value, metadata)
        % metadata.string(
            JSONPointer("/lue/value/fillcolor"), fillcolor(value))
    );
}


void to_dot(
    Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    dump_node(value, stream, metadata);
}


void dump_node(
    Property const& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        shape="%3%"
        fillcolor="%4%"
    ];
)")
        % dot_name(property)
        % property.id().name()
        % shape(property, metadata)
        % metadata.string(
            JSONPointer("/lue/property/fillcolor"), fillcolor(property))
    );
}


void to_dot(
    Property const& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    dump_node(property, stream, metadata);
    dump_node(property.values(), stream, metadata);

    link_nodes(property, property.values(), stream, metadata);
}

}  // namespace same_shape


namespace different_shape {

void dump_node(
    Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    std::string const label = metadata.boolean(
            JSONPointer("/lue/value/show_details"), false)
        ? to_dot(Table{
                Record{ value.id().name(), "" },
                Record{ "nr_items", std::to_string(value.nr_items()) },
                Record{ "rank", std::to_string(value.rank()) },
                Record{ "file_datatype",
                    hdf5::standard_datatype_as_string(
                        (value.file_datatype())) },
                Record{ "memory_datatype",
                    hdf5::native_datatype_as_string(
                        (value.memory_datatype())) }
            })
        : value.id().name()
        ;

    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        shape="%3%"
        fillcolor="%4%"
    ];
)")
        % dot_name(value)
        % label
        % shape(value, metadata)
        % metadata.string(
            JSONPointer("/lue/value/fillcolor"), fillcolor(value))
    );
}


void to_dot(
    Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    dump_node(value, stream, metadata);
}


void dump_node(
    Property const& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        shape="%3%"
        fillcolor="%4%"
    ];
)")
        % dot_name(property)
        % property.id().name()
        % shape(property, metadata)
        % metadata.string(
            JSONPointer("/lue/property/fillcolor"), fillcolor(property))
    );
}


void to_dot(
    Property const& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    dump_node(property, stream, metadata);
    dump_node(property.values(), stream, metadata);

    link_nodes(property, property.values(), stream, metadata);

    if(property.space_is_discretized()) {
        link_nodes(property, property.space_discretization(),
            stream, metadata, 0.0, "dashed");
    }
}

}  // namespace different_shape


void to_dot(
    Property const& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    auto const& configuration = property.configuration();

    switch(configuration.shape_per_item_type()) {
        case ShapePerItemType::same: {
            auto file_datatype = same_shape::Property::file_datatype(
                property.id());
            to_dot(same_shape::Property(property.id(),
                memory_datatype(file_datatype)), stream, metadata);
            break;
        }
        case ShapePerItemType::different: {
            auto file_datatype = different_shape::Property::file_datatype(
                property.id());
            to_dot(different_shape::Property(property.id(),
                memory_datatype(file_datatype)), stream, metadata);
            break;
        }
    }
}


// void to_dot(
//     Domain const& domain,
//     std::ostream& stream,
//     Metadata const& metadata)
// {
//     dump_node(domain, stream, metadata);
// 
//     if(space_domain_exists(domain)) {
//         auto const space_domain = SpaceDomain(domain);
// 
//         dump_node(space_domain, stream, metadata);
//         link_nodes(domain, space_domain, stream, metadata);
//     }
// }


// void dump_node(
//     PropertySet const& property_set,
//     std::ostream& stream,
//     Metadata const& metadata)
// {
//     stream << boost::str(boost::format(R"(
//     %1% [
//         label=<%2%>
//         shape="%3%"
//         fillcolor="%4%"
//     ];
// )")
//         % dot_name(property_set)
//         % property_set.id().name()
//         % shape(property_set, metadata)
//         % metadata.string(
//             JSONPointer("/lue/property_set/fillcolor"),
//             fillcolor(property_set))
//     );
// }


void to_dot(
    PropertySet const& property_set,
    std::ostream& stream,
    Metadata const& metadata)
{
    auto const& domain = property_set.domain();
    auto const& properties = property_set.properties();
    std::vector<std::string> property_dot_names;

    for(auto const& name: properties.names()) {
        auto const& property = properties[name];

        to_dot(property, stream, metadata);
        link_nodes(property_set, property, stream, metadata);
        link_nodes(property, domain, stream, metadata);

        property_dot_names.push_back(dot_name(property));
    }

    stream << boost::str(boost::format(R"(
    { rank=same %1% }
)")
        % boost::join(property_dot_names, " ")
        );
}

}  // namespace omnipresent


namespace located {

// namespace same_shape {
// 
// }  // namespace same_shape
// 
// 
// namespace different_shape {
// 
// }  // namespace same_shape



void to_dot(
    Property const& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    auto const& configuration = property.configuration();

    switch(configuration.shape_per_item_type()) {

        case ShapePerItemType::same: {
            // auto file_datatype = same_shape::Property::file_datatype(
            //     property.id());
            // to_dot(same_shape::Property(property,
            //     memory_datatype(file_datatype)), stream, metadata);
            break;
        }

        case ShapePerItemType::different: {
            // auto file_datatype = different_shape::Property::file_datatype(
            //     property.id());
            // to_dot(different_shape::Property(property,
            //     memory_datatype(file_datatype)), stream, metadata);
            break;
        }

    }
}

namespace shared {

void to_dot(
    PropertySet const& property_set,
    std::ostream& stream,
    Metadata const& metadata)
{
    auto const& domain = property_set.domain();
    auto const& properties = property_set.properties();
    std::vector<std::string> property_dot_names;

    for(auto const& name: properties.names()) {
        auto const property = located::Property(properties[name].id());

        switch(property.configuration2().shape_variability()) {

            case lue::time::PropertyConfiguration::ShapeVariability::constant: {
                to_dot(
                    constant_shape::Property(property.id()),
                    stream, metadata);
                break;
            }
        }

        link_nodes(property_set, property, stream, metadata);
        link_nodes(property, domain, stream, metadata);

        property_dot_names.push_back(dot_name(property));
    }

    stream << boost::str(boost::format(R"(
    { rank=same %1% }
)")
        % boost::join(property_dot_names, " ")
        );
}

}  // namespace shared


void to_dot(
    PropertySet const& property_set,
    std::ostream& stream,
    Metadata const& metadata)
{
    auto const& domain = property_set.domain();

    TimeDomain time_domain(domain.id());

    switch(time_domain.configuration().ownership()) {

        case TimeDomain::Configuration::Ownership::shared: {
            to_dot(
                shared::PropertySet(property_set.id()),
                stream, metadata);
            break;
        }
    }
}

}  // namespace located
}  // namespace time


void dump_node(
    PropertySet const& property_set,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        shape="%3%"
        fillcolor="%4%"
    ];
)")
        % dot_name(property_set)
        % property_set.id().name()
        % shape(property_set, metadata)
        % metadata.string(
            JSONPointer("/lue/property_set/fillcolor"),
            fillcolor(property_set))
    );
}


void to_dot(
    PropertySet const& property_set,
    std::ostream& stream,
    Metadata const& metadata)
{
    dump_node(property_set, stream, metadata);

    auto const& ids = property_set.ids();

    dump_node(ids, stream, metadata);
    link_nodes(property_set, ids, stream, metadata);


    auto const& domain = property_set.domain();

    to_dot(domain, stream, metadata);
    link_nodes(property_set, domain, stream, metadata);


    switch(property_set.domain().configuration().domain_type()) {

        case Domain::Configuration::DomainType::located: {
            to_dot(
                time::located::PropertySet(property_set.id()),
                stream, metadata);
            break;
        }

        case Domain::Configuration::DomainType::omnipresent: {

            to_dot(
                time::omnipresent::PropertySet(property_set.id()),
                stream, metadata);
            break;
        }

    }



//     auto const& domain = property_set.domain();
// 
//     to_dot(domain, stream, metadata);
//     link_nodes(property_set, domain, stream, metadata);
// 
// 
//     auto const& properties = property_set.properties();
//     std::vector<std::string> property_dot_names;
// 
//     for(auto const& name: properties.names()) {
//         auto const& property = properties[name];
// 
//         to_dot(property, stream, metadata);
//         link_nodes(property_set, property, stream, metadata);
//         link_nodes(property, domain, stream, metadata);
// 
//         property_dot_names.push_back(dot_name(property));
//     }
// 
//     stream << boost::str(boost::format(R"(
//     { rank=same %1% }
// )")
//         % boost::join(property_dot_names, " ")
//         );
}

}  // namespace constant_size


/*!
    @brief      Translate the structure of a property-set to a DOT graph
*/
void to_dot(
    PropertySet const& property_set,
    std::ostream& stream,
    Metadata const& metadata)
{
    // Subgraph graph(stream, property_set.id());

    auto configuration = property_set.configuration();

    switch(configuration.size_of_item_collection_type()) {

        case(SizeOfItemCollectionType::constant_size): {

            to_dot(
                constant_size::PropertySet(property_set.id()),
                stream, metadata);

        }
    }
}


/*!
    @brief      Translate the structure of a phenomenon to a DOT graph
*/
void to_dot(
    Phenomenon const& phenomenon,
    std::ostream& stream,
    Metadata const& metadata)
{
    dump_node(phenomenon, stream, metadata);

    auto const& property_sets = phenomenon.property_sets();

    for(auto const& name: property_sets.names()) {
        auto const& property_set = property_sets[name];

        to_dot(property_set, stream, metadata);
        link_nodes(phenomenon, property_set, stream, metadata);
    }
}


/*!
    @brief      Translate the structure of a universe to a DOT graph
*/
void to_dot(
    Universe const& universe,
    std::ostream& stream,
    Metadata const& metadata)
{
    auto const& phenomena = universe.phenomena();

    for(auto const& name: phenomena.names()) {
        to_dot(phenomena[name], stream, metadata);
    }
}


/*!
    @brief      Translate the structure of a LUE dataset to a DOT graph

    Each dataset ends up in a sub-graph, to set it apart from other
    datasets.
*/
void to_dot(
    Dataset const& dataset,
    std::ostream& stream,
    Metadata const& metadata)
{
    Subgraph graph(stream, dataset.id(), dataset.pathname());

    auto const& universes = dataset.universes();

    for(auto const& name: universes.names()) {
        to_dot(universes[name], stream, metadata);
    }

    auto const& phenomena = dataset.phenomena();

    for(auto const& name: phenomena.names()) {
        to_dot(phenomena[name], stream, metadata);
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
    assert(!datasets.empty());

    // Create a DOT graph representation of the dataset. We don't have to
    // write the data values. We do have to write the organization, in HDF5,
    // of the data values.

    stream << boost::str(boost::format(R"(digraph {
    fontname="Courier"
    fontsize=10

    graph [
        bgcolor="transparent"
    ];

    node [
        fontname="Courier"
        fontsize=10
        fontcolor="%1%"
        style="filled"
    ];

    edge [
        fontname="Courier"
        fontsize=10
        arrowhead="open"
        arrowtail="open"
    ];

)")
        % metadata.string(
            JSONPointer("/dot/node/fontcolor"), "black")
    );

    for(auto const& dataset: datasets) {
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
    Dataset const& dataset,
    std::ostream& stream,
    Metadata const& metadata)
{
    to_dot(DatasetRefs{dataset}, stream, metadata);
}


/*!
    @brief      Translate the structure of a LUE dataset to a DOT graph
*/
void translate_lue_dataset_to_dot(
    Dataset const& dataset,
    std::string const& dot_filename,
    Metadata const& metadata)
{
    translate_lue_datasets_to_dot(
        DatasetRefs{dataset}, dot_filename, metadata);
}

}  // namespace utility
}  // namespace lue
