#include "lue/utility/lue_dump/txt_format.h"
#include "lue/cxx_api.h"


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


// namespace variable_size {
// 
// }  // variable_size


void describe_property_set(
    PropertySet const& property_set,
    std::ostream& stream,
    size_t indentation)
{
    print_message(stream, indentation, "- " + property_set.name());

    ++indentation;

    auto const size_of_item_collection_type =
        property_set.configuration().size_of_item_collection_type();

    // switch(size_of_item_collection_type) {
    //     case(SizeOfItemCollectionType::constant_size): {
    //         describe_property_set(constant_size::PropertySet(property_set),
    //             stream, indentation);

    //         // constant_size::describe_property_set(property_set, stream,
    //         //     indentation);

    //         break;
    //     }
    // }
}


void describe_property_sets(
    PropertySets const& property_sets,
    std::ostream& stream,
    size_t indentation)
{
    print_message(stream, indentation, "property sets: ");

    for(auto const& name: property_sets.names()) {
        describe_property_set(property_sets[name], stream, indentation);
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
    print_message(stream, indentation, "phenomena: ");

    for(auto const& name: phenomena.names()) {
        describe_phenomenon(phenomena[name], stream, indentation);
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
    print_message(stream, indentation, "universes: ");

    for(auto const& name: universes.names()) {
        describe_universe(universes[name], stream, indentation);
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
