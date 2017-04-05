#include "lue_translate/dot.h"
#include "lue/constant_size/time/omnipresent/different_shape/property.h"
#include "lue/constant_size/time/omnipresent/property_set.h"
#include "lue/constant_size/time/omnipresent/same_shape/property.h"
#include <boost/format.hpp>
#include <exception>
#include <fstream>
#include <functional>


namespace lue {

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

namespace constant_size {
namespace time {
namespace omnipresent {

void to_dot(
    Domain const& domain,
    std::ostream& stream)
{
    Subgraph graph(stream, domain.id());

    stream << "domain";

}


void to_dot(
    Property const& property,
    std::ostream& stream)
{
    Subgraph graph(stream, property.id());

    stream << "property";

    // switch(configuration.shape_per_item_type()) {
    //     case ShapePerItemType::same: {
    //         auto file_datatype =
    //             same_shape::Property::file_datatype(
    //                 property.id());
    //         object = py::cast(new same_shape::Property(
    //             property, memory_datatype(file_datatype)));
    //         break;
    //     }
    //     case ShapePerItemType::different: {
    //         auto file_datatype =
    //             different_shape::Property::file_datatype(
    //                 property.id());
    //         object = py::cast(new different_shape::Property(
    //             property, memory_datatype(file_datatype)));
    //         break;
    //     }
    // }

    // for(auto const& name: properties.names()) {
    //     to_dot(property[name], stream);
    // }
}


void to_dot(
    Properties const& properties,
    std::ostream& stream)
{
    Subgraph graph(stream, properties.id());

    for(auto const& name: properties.names()) {
        to_dot(properties[name], stream);
    }
}


void to_dot(
    PropertySet const& property_set,
    std::ostream& stream)
{
    to_dot(property_set.domain(), stream);
    to_dot(property_set.properties(), stream);
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size


void to_dot(
    PropertySet const& property_set,
    std::ostream& stream)
{
    Subgraph graph(stream, property_set.id());

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
    PropertySets const& property_sets,
    std::ostream& stream)
{
    Subgraph graph(stream, property_sets.id());

    for(auto const& name: property_sets.names()) {
        to_dot(property_sets[name], stream);
    }
}


void to_dot(
    Phenomenon const& phenomenon,
    std::ostream& stream)
{
    Subgraph graph(stream, phenomenon.id());

    to_dot(phenomenon.property_sets(), stream);
}


void to_dot(
    Phenomena const& phenomena,
    std::ostream& stream)
{
    Subgraph graph(stream, phenomena.id());

    for(auto const& name: phenomena.names()) {
        to_dot(phenomena[name], stream);
    }
}


void to_dot(
    Universe const& universe,
    std::ostream& stream)
{
    Subgraph graph(stream, universe.id());

    to_dot(universe.phenomena(), stream);
}


void to_dot(
    Universes const& universes,
    std::ostream& stream)
{
    Subgraph graph(stream, universes.id());

    for(auto const& name: universes.names()) {
        to_dot(universes[name], stream);
    }
}


void to_dot(
    Dataset const& dataset,
    std::ostream& stream)
{
    Subgraph graph(stream, dataset.id(), dataset.pathname());

    to_dot(dataset.universes(), stream);
    to_dot(dataset.phenomena(), stream);
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


} // namespace lue
