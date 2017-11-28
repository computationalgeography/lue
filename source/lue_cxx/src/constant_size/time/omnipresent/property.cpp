#include "lue/constant_size/time/omnipresent/property.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

Property::Property(
    hdf5::Group&& group)

    : constant_size::Property(std::forward<hdf5::Group>(group))

{
}


Property create_property(
    hdf5::Group& group,
    std::string const& name,
    Property::Configuration const& configuration)
{
    return Property{
        constant_size::create_property(group, name, configuration)};
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
