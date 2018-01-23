#include "lue/constant_size/property.hpp"


namespace lue {
namespace constant_size {

Property::Property(
    hdf5::Group&& group)

    : lue::Property{std::forward<hdf5::Group>(group)}

{
}


Property create_property(
    hdf5::Group& parent,
    std::string const& name,
    Configuration const& configuration)
{
    auto property = lue::create_property(parent, name, configuration);

    return Property{std::move(property)};
}

}  // namespace constant_size
}  // namespace lue
