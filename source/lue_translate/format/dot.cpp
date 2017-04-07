#include "lue_translate/format/dot.h"
#include "lue/constant_size/time/omnipresent/different_shape/property.h"
#include "lue/constant_size/time/omnipresent/property_set.h"
#include "lue/constant_size/time/omnipresent/same_shape/property.h"
#include <boost/format.hpp>
#include <exception>
#include <fstream>
#include <functional>


namespace lue {
namespace utility {

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
            % id
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
    std::ostream& stream)
{
    stream << boost::str(boost::format(R"(
    node_%1% [
        label=<%2%>
        shape="box"
    ];
)")
        % dataset.id()
        % dataset.id().name()
        );
}


void dump_node(
    hdf5::Group const& group,
    std::ostream& stream)
{
    stream << boost::str(boost::format(R"(
    node_%1% [
        label=<%2%>
    ];
)")
        % group.id()
        % group.id().name()
        );
}


template<
    typename T1,
    typename T2>
void link_nodes(
    T1 const& from_object,
    T2 const& to_object,
    std::ostream& stream)
{
    stream << boost::str(boost::format(R"(
    node_%1% -> node_%2% [
    ];
)")
        % from_object.id()
        % to_object.id()
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
    std::ostream& stream)
{
    dump_node(value, stream);
}


void to_dot(
    Property const& property,
    std::ostream& stream)
{
    dump_node(property, stream);
    dump_node(property.values(), stream);

    link_nodes(property, property.values(), stream);
}

}  // namespace same_shape


namespace different_shape {

void to_dot(
    Value const& value,
    std::ostream& stream)
{
    dump_node(value, stream);
}


void to_dot(
    Property const& property,
    std::ostream& stream)
{
    dump_node(property, stream);
    dump_node(property.values(), stream);

    link_nodes(property, property.values(), stream);
}

}  // namespace different_shape


void to_dot(
    Property const& property,
    std::ostream& stream)
{
    auto const& configuration = property.configuration();

    switch(configuration.shape_per_item_type()) {
        case ShapePerItemType::same: {
            auto file_datatype =
                same_shape::Property::file_datatype(
                    property.id());
            to_dot(same_shape::Property(property,
                memory_datatype(file_datatype)), stream);
            break;
        }
        case ShapePerItemType::different: {
            auto file_datatype =
                different_shape::Property::file_datatype(
                    property.id());
            to_dot(different_shape::Property(property,
                memory_datatype(file_datatype)), stream);
            break;
        }
    }
}


void to_dot(
    PropertySet const& property_set,
    std::ostream& stream)
{
    dump_node(property_set, stream);

    auto const& domain = property_set.domain();

    {
        // Subgraph graph(stream, domain.id());

        dump_node(domain, stream);
    }

    auto const& properties = property_set.properties();

    for(auto const& name: properties.names()) {
        auto const& property = properties[name];

        to_dot(property, stream);
        link_nodes(property_set, property, stream);
        link_nodes(property, domain, stream);
    }
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size


void to_dot(
    PropertySet const& property_set,
    std::ostream& stream)
{
    // Subgraph graph(stream, property_set.id());

    auto configuration = property_set.configuration();

    switch(configuration.size_of_item_collection_type()) {
        // TODO Switch on time domain, once possible.

        case(SizeOfItemCollectionType::constant_size): {
            to_dot(constant_size::time::omnipresent::PropertySet(
                property_set.id()), stream);
            break;
        }
    }
}


void to_dot(
    Phenomenon const& phenomenon,
    std::ostream& stream)
{
    dump_node(phenomenon, stream);

    auto const& property_sets = phenomenon.property_sets();

    for(auto const& name: property_sets.names()) {
        auto const& property_set = property_sets[name];

        to_dot(property_set, stream);
        link_nodes(phenomenon, property_set, stream);
    }
}


void to_dot(
    Universe const& universe,
    std::ostream& stream)
{
    auto const& phenomena = universe.phenomena();

    for(auto const& name: phenomena.names()) {
        to_dot(phenomena[name], stream);
    }
}


void to_dot(
    Dataset const& dataset,
    std::ostream& stream)
{
    Subgraph graph(stream, dataset.id(), dataset.pathname());

    auto const& universes = dataset.universes();

    for(auto const& name: universes.names()) {
        to_dot(universes[name], stream);
    }

    auto const& phenomena = dataset.phenomena();

    for(auto const& name: phenomena.names()) {
        to_dot(phenomena[name], stream);
    }
}


void to_dot(
    DatasetRefs const& datasets,
    std::ostream& stream)
{
    assert(!datasets.empty());

    // Create a DOT graph representation of the dataset. We don't have to
    // write the data values. We do have to write the organization, in HDF5,
    // of the data values.

    stream << boost::str(boost::format(R"(digraph g {
    fontname="Courier"
    fontsize=10
    label=<%1%>

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
        // TODO Join names of all datasets.
        % datasets[0].get().pathname()
        );

    for(auto const& dataset: datasets) {
        to_dot(dataset, stream);
    }

    stream << R"(}
)";
}


namespace utility {

void translate_lue_datasets_to_dot(
    DatasetRefs const& datasets,
    std::string const& dot_filename)
{
    std::ofstream stream(dot_filename);

    if(!stream) {
        throw std::runtime_error("cannot open file " + dot_filename);
    }

    to_dot(datasets, stream);
}


void translate_lue_dataset_to_dot(
    Dataset const& dataset,
    std::ostream& stream)
{
    to_dot(DatasetRefs{dataset}, stream);
}


void translate_lue_dataset_to_dot(
    Dataset const& dataset,
    std::string const& dot_filename)
{
    translate_lue_datasets_to_dot(DatasetRefs{dataset}, dot_filename);
}

}  // namespace utility
}  // namespace lue
