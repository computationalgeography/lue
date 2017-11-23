#include "lue/constant_size/time/omnipresent/property.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

Property::Property(
    hdf5::Identifier const& id)

    : constant_size::Property(id)

{
}


Property create_property(
    hdf5::Group& group,
    std::string const& name,
    Property::Configuration const& configuration)
{
    return Property{
        constant_size::create_property(group, name, configuration).id()};
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
