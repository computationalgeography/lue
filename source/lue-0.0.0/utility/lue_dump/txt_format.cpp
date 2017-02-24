#include "lue/utility/lue_dump/txt_format.h"
#include "lue/cxx_api/hdf5.h"
#include "lue/cxx_api.h"
#include <boost/algorithm/string/join.hpp>


namespace lue {
namespace {

size_t const tab_size = 4;


std::string indent(
    size_t indentation)
{
    return std::string(tab_size * indentation, ' ');
}


void print_message(
    std::ostream& stream,
    size_t indentation,
    std::string const& message)
{
    stream << indent(indentation) <<  message << "\n";
}


// namespace constant_size {

// void describe_property(
//     Property const& property,
//     std::ostream& stream,
//     size_t indentation)
// {
//     print_message(stream, indentation, "- " + property.name());
// 
//     ++indentation;
// 
//     // auto const& domain = property.domain();
//     // auto const& value = property.value();
// 
// }
// 
// 
// void describe_properties(
//     Properties const& properties,
//     std::ostream& stream,
//     size_t indentation)
// {
//     print_message(stream, indentation, "properties: ");
// 
//     for(auto const& name: properties.names()) {
//         describe_property(properties[name], stream, indentation);
//     }
// }
// 
// 
// void describe_time_domain(
//     TimeDomain const& time_domain,
//     std::ostream& stream,
//     size_t indentation)
// {
//     print_message(stream, indentation, "time domain: ");
// 
//     auto const& configuration = time_domain.configuration();
//     auto time_domain_type = configuration.type();
//     auto time_domain_item_type = configuration.item_type();
// 
//     switch(time_domain_type) {
//         case TimeDomainType::omnipresent: {
//             break;
//         }
//         case TimeDomainType::shared: {
//             break;
//         }
//     }
// }
// 
// 
// void describe_space_domain(
//     SpaceDomain const& space_domain,
//     std::ostream& stream,
//     size_t indentation)
// {
//     print_message(stream, indentation, "space domain: ");
// }
// 
// 
// void describe_domain(
//     Domain const& domain,
//     std::ostream& stream,
//     size_t indentation)
// {
//     print_message(stream, indentation, "domain: ");
// 
//     ++indentation;
// 
//     describe_time_domain(domain.time_domain(), stream, indentation);
//     describe_space_domain(domain.space_domain(), stream, indentation);
// }


// namespace omnipresent {




// void describe_property_set(
//     PropertySet const& property_set,
//     std::ostream& stream,
//     size_t indentation)
// {
//     
// 
// }
// 
// }  // namespace omnipresent
// 
// 
// namespace shared {
// 
// 
// 
// 
// void describe_property_set(
//     PropertySet const& property_set,
//     std::ostream& stream,
//     size_t indentation)
// {
// 
// 
// }
// 
// }  // namespace shared


// void describe_property_set(
//     PropertySet const& property_set,
//     std::ostream& stream,
//     size_t indentation)
// {
//     auto const& time_domain = property_set.domain().time_domain();
//     auto const time_domain_type = time_domain.configuration().type();
//     auto const time_domain_item_type = time_domain.configuration().item_type();
// 
//     print_message(stream, indentation, "time:");
// 
//     ++indentation;
// 
//     print_message(stream, indentation, "domain type: " +
//         time_domain_type_to_string(time_domain_type));
//     print_message(stream, indentation, "domain item type: " +
//         time_domain_item_type_to_string(time_domain_item_type));
// 
//     switch(time_domain_type) {
//         case TimeDomainType::omnipresent: {
//             omnipresent::describe_property_set(property_set, stream,
//                 indentation);
//             break;
//         }
//         case TimeDomainType::shared: {
//             shared::describe_property_set(property_set, stream,
//                 indentation);
//             break;
//         }
//     }
// }
// 
// }  // Namespace constant_size


// void describe_property_set(
//     constant_size::time::omnipresent::Properties const& properties,
//     std::ostream& stream,
//     size_t indentation)
// {
// 
// }


// void describe_property(
//     Property const& property,
//     std::ostream& stream,
//     size_t indentation)
// {
//     print_message(stream, indentation, "- " + property.name());
// }


// void describe_property(
//     constant_size::time::Property const& property,
//     std::ostream& stream,
//     size_t indentation)
// {
//     using namespace constant_size;
// 
//     describe_property(property.group(), stream, indentation);
// }
// 
// 
// void describe_property(
//     constant_size::time::omnipresent::Property const& property,
//     std::ostream& stream,
//     size_t indentation)
// {
//     using namespace constant_size;
// 
//     describe_property(dynamic_cast<time::Property const&>(property),
//         stream, indentation);
// }


// void describe_shape(
//     Shape const& shape,
//     std::ostream& stream,
//     size_t indentation)
// {
//     std::vector<std::string> strings(shape.size());
//     std::transform(shape.begin(), shape.end(), strings.begin(),
//             [](Shape::value_type const& extent) {
//         return std::to_string(extent);
//     });
// 
//     print_message(stream, indentation,
//         "shape: (" + boost::algorithm::join(strings, ", ") + ")");
// }


// void describe_value_type(
//     hid_t const type_id,
//     std::ostream& stream,
//     size_t indentation)
// {
//     print_message(stream, indentation, "value type: " +
//         hdf5::standard_datatype_as_string(type_id));
// }


// void describe_value_type(
//     hdf5::Identifier const& type_id,
//     std::ostream& stream,
//     size_t indentation)
// {
//     print_message(stream, indentation, "value type: " +
//         hdf5::standard_datatype_as_string(type_id));
// }


// void describe_item(
//     constant_size::time::omnipresent::same_shape::Item const& item,
//     std::ostream& stream,
//     size_t indentation)
// {
//     assert(item.shape().size() > 1);
// 
//     print_message(stream, indentation,
//         "nr_items: " + std::to_string(item.shape()[0]));
// 
//     auto const first = item.shape().begin() + 1;
//     auto const last = item.shape().end();
//     describe_shape(Shape(first, last), stream, indentation);
//     describe_value_type(item.type_id(), stream, indentation);
// }


// void describe_item(
//     constant_size::time::omnipresent::different_shape::Item const& item,
//     std::ostream& stream,
//     size_t indentation)
// {
//     print_message(stream, indentation,
//         "nr_items: " + std::to_string(item.nr_items()));
//     print_message(stream, indentation,
//         "rank: " + std::to_string(item.rank()));
//     describe_value_type(item.type_id(), stream, indentation);
// }


// void describe_property(
//     constant_size::time::omnipresent::same_shape::Property const& property,
//     std::ostream& stream,
//     size_t indentation)
// {
//     describe_item(property.values(), stream, indentation);
// }


// void describe_property(
//     constant_size::time::omnipresent::different_shape::Property const& property,
//     std::ostream& stream,
//     size_t indentation)
// {
//     describe_item(property.values(), stream, indentation);
// }


// void describe_domain(
//     constant_size::time::omnipresent::Domain const& /* domain */,
//     std::ostream& /* stream */,
//     size_t /* indentation */)
// {
//     // describe_space_domain(domain.space_domain(), stream, indentation);
// }


void describe_property_set(
    constant_size::time::omnipresent::PropertySet const& property_set,
    std::ostream& /* stream */,
    size_t /* indentation */)
{
    using namespace constant_size::time::omnipresent;

    // TODO Hier verder: property_set moet
    //     constant_size::time::omnipresent::Domain teruggeven
    // describe_domain(property_set.domain(), stream, indentation);

    auto const& properties = property_set.properties();

    if(!properties.empty()) {
        // print_message(stream, indentation, "properties: ");

        // for(auto const& name: properties.names()) {

        //     // auto const& property = properties[name];

        //     // TODO Overload
        //     // describe_property(property, stream, indentation);

        //     ++indentation;

        //     // TODO property is a specialized type now.
        //     // auto const& value = property.value();
        //     // auto const shape_per_item = value.configuration().shape_per_item();

        //     // print_message(stream, indentation, "shape_per_item: " +
        //     //         shape_per_item_to_string(shape_per_item));

        //     // switch(shape_per_item) {
        //     //     case ShapePerItem::same: {
        //     //         describe_property(
        //     //             same_shape::Property(const_cast<lue::Property&>(
        //     //                 property)), stream, indentation);
        //     //         break;
        //     //     }
        //     //     case ShapePerItem::different: {
        //     //         describe_property(
        //     //             different_shape::Property(const_cast<lue::Property&>(
        //     //                 property)), stream, indentation);
        //     //         break;
        //     //     }
        //     // }

        //     --indentation;
        // }
    }
}


void describe_property_set(
    constant_size::time::shared::PropertySet const& /* property_set */,
    std::ostream& /* stream */,
    size_t /* indentation */)
{
    // describe_properties(property_set.properties(), stream, indentation);
}


void describe_property_set(
    PropertySet const& property_set,
    std::ostream& stream,
    size_t indentation)
{
    print_message(stream, indentation, "- " + property_set.name());

    auto const size_of_item_collection_type =
        property_set.configuration().size_of_item_collection_type();
    auto const& time_domain = property_set.domain().time_domain();
    auto const time_domain_type = time_domain.configuration().type();
    auto const time_domain_item_type = time_domain.configuration().item_type();

    ++indentation;

    print_message(stream, indentation, "- time domain:");

    ++indentation;
    print_message(stream, indentation, "type: " +
        time_domain_type_to_string(time_domain_type));
    print_message(stream, indentation, "item type: " +
        time_domain_item_type_to_string(time_domain_item_type));
    --indentation;

    switch(size_of_item_collection_type) {
        case(SizeOfItemCollectionType::constant_size): {
            switch(time_domain_type) {
                case TimeDomainType::omnipresent: {
                    describe_property_set(
                        constant_size::time::omnipresent::PropertySet(
                            const_cast<PropertySet&>(property_set)),
                        stream, indentation);
                    break;
                }
                case TimeDomainType::shared: {
                    describe_property_set(
                        constant_size::time::shared::PropertySet(
                            const_cast<PropertySet&>(property_set)),
                        stream, indentation);
                    break;
                }
            }

            break;
        }
    }
}


void describe_property_sets(
    PropertySets const& property_sets,
    std::ostream& stream,
    size_t indentation)
{
    if(!property_sets.empty()) {
        // print_message(stream, indentation, "property sets: ");

        for(auto const& name: property_sets.names()) {
            describe_property_set(property_sets[name], stream, indentation);
        }
    }
}


void describe_phenomenon(
    Phenomenon const& phenomenon,
    std::ostream& stream,
    size_t indentation)
{
    print_message(stream, indentation, "- " + phenomenon.name());

    ++indentation;

    describe_property_sets(phenomenon.property_sets(), stream, indentation);
}


void describe_phenomena(
    Phenomena const& phenomena,
    std::ostream& stream,
    size_t indentation)
{
    if(!phenomena.empty()) {
        // print_message(stream, indentation, "phenomena: ");

        for(auto const& name: phenomena.names()) {
            describe_phenomenon(phenomena[name], stream, indentation);
        }
    }
}


void describe_universe(
    Universe const& universe,
    std::ostream& stream,
    size_t indentation)
{
    print_message(stream, indentation, "- " + universe.name());

    ++indentation;

    describe_phenomena(universe.phenomena(), stream, indentation);

}


void describe_universes(
    Universes const& universes,
    std::ostream& stream,
    size_t indentation)
{
    if(!universes.empty()) {
        // print_message(stream, indentation, "universes: ");

        for(auto const& name: universes.names()) {
            describe_universe(universes[name], stream, indentation);
        }
    }
}


void describe_dataset(
    Dataset const& dataset,
    std::ostream& stream,
    size_t indentation)
{
    print_message(stream, indentation, dataset.pathname());

    describe_universes(dataset.universes(), stream, indentation);
    describe_phenomena(dataset.phenomena(), stream, indentation);
}

}  // Anonymous namespace


void describe_dataset(
    Dataset const& dataset,
    std::ostream& stream)
{
    describe_dataset(dataset, stream, 0);
}

}  // namespace lue
