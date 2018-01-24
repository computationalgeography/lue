#include "lue/constant_collection/time/located/constant_shape/property.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {
namespace constant_shape {

Property::Property(
    hdf5::Group&& group)

    : located::Property{std::forward<hdf5::Group>(group)}

{
}


Property create_property(
    hdf5::Group& group,
    std::string const& name,
    Configuration const& configuration)
{
    auto property = located::create_property(group, name, configuration);

    return Property(hdf5::Group{property.id()});
}

}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
