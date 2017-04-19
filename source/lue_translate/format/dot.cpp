#include "lue_translate/format/dot.h"
#include "lue/constant_size/time/omnipresent/different_shape/property.h"
#include "lue/constant_size/time/omnipresent/property_set.h"
#include "lue/constant_size/time/omnipresent/same_shape/property.h"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <fstream>
#include <functional>


namespace lue {
namespace utility {

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
    hdf5::Dataset const& dataset,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        shape="box"
        fillcolor="%3%"
    ];
)")
        % dot_name(dataset)
        % dataset.id().name()
        % metadata.value("/dataset/fillcolor", "grey")
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
        fillcolor="%3%"
    ];
)")
        % dot_name(domain)
        % domain.id().name()
        % metadata.value("/domain/fillcolor", "grey")
        );
}


void dump_node(
    Property const& property,
    std::ostream& stream,
    Metadata const& metadata)
{
    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        fillcolor="%3%"
    ];
)")
        % dot_name(property)
        % property.id().name()
        % metadata.value("/property/fillcolor", "grey")
        );
}


// TODO Once all specialized values inherit from value, this will work

// void dump_node(
//     Value const& value,
//     std::ostream& stream,
//     Metadata const& metadata)
// {
//     stream << boost::str(boost::format(R"(
//     %1% [
//         label=<%2%>
//         fillcolor="%3%"
//     ];
// )")
//         % dot_name(value)
//         % value.id().name()
//         % metadata.value("/value/fillcolor", "grey")
//         );
// }


void dump_node(
    hdf5::Group const& group,
    std::ostream& stream,
    Metadata const& /* metadata */)
{
    stream << boost::str(boost::format(R"(
    %1% [
        label=<%2%>
        fillcolor="%3%"
    ];
)")
        % dot_name(group)
        % group.id().name()
        % "grey"
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
namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {

void to_dot(
    Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    dump_node(value, stream, metadata);
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

void to_dot(
    Value const& value,
    std::ostream& stream,
    Metadata const& metadata)
{
    dump_node(value, stream, metadata);
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
            to_dot(same_shape::Property(property,
                memory_datatype(file_datatype)), stream, metadata);
            break;
        }
        case ShapePerItemType::different: {
            auto file_datatype = different_shape::Property::file_datatype(
                property.id());
            to_dot(different_shape::Property(property,
                memory_datatype(file_datatype)), stream, metadata);
            break;
        }
    }
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

    dump_node(domain, stream, metadata);
    link_nodes(property_set, domain, stream, metadata);


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
}  // namespace time
}  // namespace constant_size


void to_dot(
    PropertySet const& property_set,
    std::ostream& stream,
    Metadata const& metadata)
{
    // Subgraph graph(stream, property_set.id());

    auto configuration = property_set.configuration();

    switch(configuration.size_of_item_collection_type()) {
        // TODO Switch on time domain, once possible.

        case(SizeOfItemCollectionType::constant_size): {
            to_dot(constant_size::time::omnipresent::PropertySet(
                property_set.id()), stream, metadata);
            break;
        }
    }
}


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

    node [
        fontname="Courier"
        fontsize=10
        style="filled"
    ];

    edge [
        fontname="Courier"
        fontsize=10
        arrowhead="open"
        arrowtail="open"
    ];

)")
        );

    for(auto const& dataset: datasets) {
        to_dot(dataset, stream, metadata);
    }

    stream << R"(}
)";
}


namespace utility {

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


void translate_lue_dataset_to_dot(
    Dataset const& dataset,
    std::ostream& stream,
    Metadata const& metadata)
{
    to_dot(DatasetRefs{dataset}, stream, metadata);
}


void translate_lue_dataset_to_dot(
    Dataset const& dataset,
    std::string const& dot_filename,
    Metadata const& metadata)
{
    translate_lue_datasets_to_dot(DatasetRefs{dataset}, dot_filename, metadata);
}

}  // namespace utility
}  // namespace lue
