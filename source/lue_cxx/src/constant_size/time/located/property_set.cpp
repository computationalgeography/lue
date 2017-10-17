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


PropertySet::PropertySet(
    constant_size::PropertySet&& property_set)

    : constant_size::PropertySet(std::forward<constant_size::PropertySet>(
        property_set)),
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
    hdf5::Group& group,
    std::string const& name)
{
    auto property_set = constant_size::create_property_set(group, name,
        Domain::Configuration(
            Domain::Configuration::DomainType::located)
    );

    omnipresent::same_shape::create_value(
        property_set.id(), ids_tag, H5T_STD_U64LE, H5T_NATIVE_HSIZE);

    return PropertySet(std::move(property_set));
}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
