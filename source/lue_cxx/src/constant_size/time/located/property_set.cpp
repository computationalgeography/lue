#include "lue/constant_size/time/located/property_set.hpp"
// #include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

PropertySet::PropertySet(
    hdf5::Group&& group)

    : constant_size::PropertySet(std::forward<hdf5::Group>(group)),
      _domain{*this}

{
}


Domain const& PropertySet::domain() const
{
    return _domain;
}


Domain& PropertySet::domain()
{
    return _domain;
}


PropertySet create_property_set(
    PropertySets& property_sets,
    std::string const& name)
{
    auto& property_set = property_sets.add(name,
        constant_size::create_property_set(property_sets, name,
            Domain::Configuration(
                Domain::Configuration::DomainType::located)
            )
        );

    return PropertySet(hdf5::Group{property_set.id()});
}


PropertySet create_property_set(
    PropertySets& property_sets,
    std::string const& name,
    PropertySet::Ids const& ids)
{
    auto& property_set = property_sets.add(name,
        constant_size::create_property_set(property_sets, name, ids,
            Domain::Configuration(
                Domain::Configuration::DomainType::located)
            )
        );

    return PropertySet(hdf5::Group{property_set.id()});
}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
