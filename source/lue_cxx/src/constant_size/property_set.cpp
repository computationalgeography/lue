#include "lue/constant_size/property_set.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {

// PropertySet::PropertySet(
//     Phenomenon& phenomenon,
//     std::string const& name)
// 
//     : lue::PropertySet(std::move(phenomenon.add_property_set(name)))
// 
// {
// }


PropertySet::PropertySet(
    hdf5::Identifier const& id)

    : lue::PropertySet{hdf5::Group{id}},
      _ids{*this, ids_tag, hdf5::Datatype{H5T_NATIVE_HSIZE}}

{
}


PropertySet::PropertySet(
    lue::PropertySet&& property_set)

    : lue::PropertySet{std::forward<lue::PropertySet>(property_set)},
      _ids{*this, ids_tag, hdf5::Datatype{H5T_NATIVE_HSIZE}}

{
}


PropertySet::Ids const& PropertySet::ids() const
{
    return _ids;
}


PropertySet::Ids& PropertySet::ids()
{
    return _ids;
}


PropertySet::Ids& PropertySet::reserve(
    hsize_t const nr_items)
{
    _ids.reserve(nr_items);

    return _ids;
}


PropertySet create_property_set(
    hdf5::Group& group,
    std::string const& name,
    Domain::Configuration const& domain_configuration)
{
    auto property_set = lue::create_property_set(
        group, name,
        PropertySet::Configuration{SizeOfItemCollectionType::constant_size},
        domain_configuration);

    constant::same_shape::create_collection(
        property_set, ids_tag, hdf5::Datatype{H5T_STD_U64LE},
        hdf5::Datatype{H5T_NATIVE_HSIZE});

    return PropertySet{std::move(property_set)};
}


PropertySet create_property_set(
    hdf5::Group& group,
    std::string const& name,
    PropertySet::Ids const& ids,
    Domain::Configuration const& domain_configuration)
{
    auto property_set = lue::create_property_set(
        group, name,
        PropertySet::Configuration{SizeOfItemCollectionType::constant_size},
        domain_configuration);

    // TODO assert
    // create_value(property_set.id(), ids_tag,
    //     H5T_STD_U64LE, H5T_NATIVE_HSIZE);

    property_set.create_hard_link(ids.id(), ids_tag);

    return PropertySet{std::move(property_set)};
}

}  // namespace constant_size
}  // namespace lue
