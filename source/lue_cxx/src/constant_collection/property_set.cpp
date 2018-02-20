#include "lue/constant_collection/property_set.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_collection {

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


PropertySet create_property_set(
    Phenomenon& phenomenon,
    std::string const& name)
{
    auto& property_set = phenomenon.property_sets().add(name,
        lue::create_property_set(
            phenomenon.property_sets(), name,
            PropertySetConfiguration{CollectionVariability::constant}
        )
    );

    lue::same_shape::constant_shape::create_continuous_value(
        property_set, ids_tag, hdf5::Datatype{H5T_STD_U64LE},
        hdf5::Datatype{H5T_NATIVE_HSIZE});

    return PropertySet{hdf5::Group{property_set.id()}};
}


PropertySet create_property_set(
    Phenomenon& phenomenon,
    std::string const& name,
    PropertySet::Ids const& ids)
{
    auto& property_set = phenomenon.property_sets().add(name,
        lue::create_property_set(
            phenomenon.property_sets(), name,
            PropertySetConfiguration{CollectionVariability::constant}
        )
    );

    // TODO assert
    // create_value(property_set.id(), ids_tag,
    //     H5T_STD_U64LE, H5T_NATIVE_HSIZE);

    property_set.create_hard_link(ids.id(), ids_tag);

    return PropertySet{hdf5::Group{property_set.id()}};
}

}  // namespace constant_collection
}  // namespace lue
