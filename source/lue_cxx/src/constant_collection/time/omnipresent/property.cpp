#include "lue/constant_collection/time/omnipresent/property.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

Property::Property(
    hdf5::Group&& group)

    : constant_collection::Property(std::forward<hdf5::Group>(group))

{
}


Property create_property(
    hdf5::Group& group,
    std::string const& name)
{
    return Property{
        constant_collection::create_property(group, name)};
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
