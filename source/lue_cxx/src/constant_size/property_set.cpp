#include "lue/constant_size/property_set.hpp"


namespace lue {
namespace constant_size {

// PropertySet::PropertySet(
//     Phenomenon& phenomenon,
//     std::string const& name)
// 
//     : lue::PropertySet(std::move(phenomenon.add_property_set(name)))
// 
// {
// }


PropertySet::PropertySet(
    hdf5::Identifier const& id)

    : lue::PropertySet(id)

{
}


PropertySet::PropertySet(
    lue::PropertySet&& property_set)

    : lue::PropertySet(std::forward<lue::PropertySet>(property_set))

{
}


PropertySet create_property_set(
    hdf5::Group& group,
    std::string const& name,
    Domain::Configuration const& domain_configuration)
{
    auto property_set = lue::create_property_set(group, name,
        PropertySet::Configuration(SizeOfItemCollectionType::constant_size),
        domain_configuration);

    return PropertySet(std::move(property_set));
}

}  // namespace constant_size
}  // namespace lue
