#include "lue/constant_size/time/located/shared/constant_shape/property.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {

Property::Property(
    hdf5::Group&& group)

    : located::Property{std::forward<hdf5::Group>(group)}

{
}


Property create_property(
    hdf5::Group& group,
    std::string const& name,
    Property::Configuration const& configuration)
{
    auto property = located::create_property(
        group, name, configuration,
        lue::time::PropertyConfiguration(
            lue::time::PropertyConfiguration::ShapeVariability::constant));

    return Property(hdf5::Group{property.id()});
}

}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
