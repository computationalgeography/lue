#include "lue/constant_size/time/located/property_set.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

PropertySet::PropertySet(
    hdf5::Identifier const& id)

    : constant_size::PropertySet(id),
      // _domain{this->id()}
      _ids(this->id(), ids_tag, H5T_NATIVE_HSIZE)

{
}


omnipresent::same_shape::Value const& PropertySet::ids() const
{
    return _ids;
}


omnipresent::same_shape::Value& PropertySet::ids()
{
    return _ids;
}


omnipresent::same_shape::Value& PropertySet::reserve(
    hsize_t const nr_items)
{
    _ids.reserve(nr_items);

    return _ids;
}


PropertySet create_property_set(
    PropertySets& property_sets,
    std::string const& name)
{
    auto& property_set = property_sets.add(name, std::move(
        constant_size::create_property_set(
            property_sets, name,
            Domain::Configuration(
                Domain::Configuration::DomainType::located))
    ));

    return PropertySet(property_set.id());
}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
