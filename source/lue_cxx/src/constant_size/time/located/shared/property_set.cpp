#include "lue/constant_size/time/located/shared/property_set.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

PropertySet::PropertySet(
    hdf5::Identifier const& id)

    : located::PropertySet(id),
      _domain{this->id()}

{
}


// omnipresent::same_shape::Value& PropertySet::ids()
// {
//     return _ids;
// }


Domain const& PropertySet::domain() const
{
    return _domain;
}


Domain& PropertySet::domain()
{
    return _domain;
}


// omnipresent::same_shape::Value& PropertySet::reserve(
//     hsize_t const nr_items)
// {
//     _ids.reserve(nr_items);
// 
//     return _ids;
// }


PropertySet create_property_set(
    PropertySets& property_sets,
    std::string const& name)
{
    auto& property_set = property_sets.add(
        name, std::move(located::create_property_set(property_sets, name)));

    omnipresent::same_shape::create_value(property_set.id(), ids_tag,
        H5T_STD_U64LE, H5T_NATIVE_HSIZE);

    return PropertySet(property_set.id());
}


// PropertySet create_property_set(
//     PropertySets& property_sets,
//     std::string const& name,
//     omnipresent::same_shape::Value const& ids)
// {
//     auto& property_sets = phenomenon.property_sets();
//     auto& property_set = property_sets.add(name, std::move(
//         lue::located::create_property_set(
//             property_sets, name,
//             Domain::Configuration(TimeDomainType::omnipresent))
//     ));
// 
//     // TODO assert
//     // omnipresent::same_shape::create_value(property_set.id(), ids_tag,
//     //     H5T_STD_U64LE, H5T_NATIVE_HSIZE);
// 
//     property_set.create_hard_link(ids.id(), ids_tag);
// 
//     return PropertySet(property_set.id());
// }

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
