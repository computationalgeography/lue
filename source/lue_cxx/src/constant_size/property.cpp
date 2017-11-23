#include "lue/constant_size/property.hpp"


namespace lue {
namespace constant_size {

Property::Property(
    hdf5::Identifier const& id)

    : lue::Property{id}

{
}


Property create_property(
    hdf5::Group& group,
    std::string const& name,
    Property::Configuration const& configuration)
{
    auto property = lue::create_property(group, name, configuration);

    return Property(property.id());
}

}  // namespace constant_size
}  // namespace lue
