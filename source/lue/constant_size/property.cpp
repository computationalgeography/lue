#include "lue/constant_size/property.h"


namespace lue {
namespace constant_size {

Property::Property(
    lue::Property&& property)

    : lue::Property(std::forward<lue::Property>(property))

{
}


Property::Property(
    lue::Property const& property)

    : lue::Property(property)

{
}


Property create_property(
    hdf5::Group& group,
    std::string const& name,
    Property::Configuration const& configuration)
{
    auto property = lue::create_property(group, name, configuration);

    return Property(std::move(property));
}

}  // namespace constant_size
}  // namespace lue
