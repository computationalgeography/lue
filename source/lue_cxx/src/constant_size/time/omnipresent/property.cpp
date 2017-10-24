#include "lue/constant_size/time/omnipresent/property.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

// Property::Property(
//     constant_size::Property&& property)
// 
//     : constant_size::Property(std::forward<constant_size::Property>(property))
// 
// {
// }


Property::Property(
    hdf5::Identifier const& id)

    : constant_size::Property(id)

{
}


Property::Property(
    lue::Property const& property)

    : constant_size::Property(property)

{
}


Property create_property(
    // PropertySet& property_set,
    hdf5::Group& group,
    std::string const& name,
    Property::Configuration const& configuration)
{
    return constant_size::create_property(group, name, configuration);
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
