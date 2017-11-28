#include "lue/constant_size/time/located/shared/property_set.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

PropertySet::PropertySet(
    hdf5::Group&& group)

    : located::PropertySet(std::forward<hdf5::Group>(group)),
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
        located::create_property_set(property_sets, name)
    );

    return PropertySet(hdf5::Group{property_set.id()});
}


PropertySet create_property_set(
    PropertySets& property_sets,
    std::string const& name,
    omnipresent::same_shape::Value const& ids)
{
    auto& property_set = property_sets.add(name,
        located::create_property_set(property_sets, name, ids)
    );

    return PropertySet(hdf5::Group{property_set.id()});
}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
