#include "lue/constant_size/time/located/property_set.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

PropertySet::PropertySet(
    hdf5::Group&& group)

    : constant_size::PropertySet(std::forward<hdf5::Group>(group))
      // _domain{this->id()}

{
}


PropertySet::PropertySet(
    constant_size::PropertySet&& property_set)

    : constant_size::PropertySet(std::forward<constant_size::PropertySet>(
        property_set))
      // _domain{this->id()}

{
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name)
{
    auto property_set = constant_size::create_property_set(parent, name,
        Domain::Configuration(
            Domain::Configuration::DomainType::located)
    );

    return PropertySet(std::move(property_set));
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    PropertySet::Ids const& ids)
{
    auto property_set = constant_size::create_property_set(parent, name, ids,
        Domain::Configuration(
            Domain::Configuration::DomainType::located)
    );

    return PropertySet(std::move(property_set));
}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
