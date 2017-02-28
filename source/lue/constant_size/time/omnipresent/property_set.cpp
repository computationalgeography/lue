#include "property_set.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

// PropertySet::PropertySet(
//     Phenomenon& phenomenon,
//     std::string const& name)
// 
//     : constant_size::PropertySet(phenomenon, name)
// 
// {
// }


/*!
    @brief      Create instance based on a identifier
    @param      id Identifier of a property set
*/
PropertySet::PropertySet(
    hdf5::Identifier const& id)

    : constant_size::PropertySet(id),
      _domain(this->id()),
      _ids(this->id(), "lue_ids", H5T_NATIVE_HSIZE)

{
}


PropertySet::PropertySet(
    constant_size::PropertySet&& property_set)

    : constant_size::PropertySet(std::forward<constant_size::PropertySet>(
        property_set)),
      _domain(id()),
      _ids(id(), "lue_ids", H5T_NATIVE_HSIZE)

{
}


same_shape::Value const& PropertySet::ids() const
{
    return _ids;
}


same_shape::Value& PropertySet::ids()
{
    return _ids;
}


same_shape::Value& PropertySet::reserve_items(
    hsize_t const nr_items)
{
    _ids.reserve_values(nr_items);

    return _ids;
}


PropertySet create_property_set(
    Phenomenon& phenomenon,
    std::string const& name)
{
    auto& property_sets = phenomenon.property_sets();
    auto& property_set = property_sets.add(name, std::move(
        lue::constant_size::create_property_set(property_sets, name)));

    create_domain(property_set.id());
    same_shape::create_value(property_set.id(), "lue_ids",
        H5T_STD_U64LE, H5T_NATIVE_HSIZE);

    return PropertySet(std::move(property_set));


    // PropertySet::Configuration configuration();

    // auto property_set = lue::constant_size::create_property_set(
    //     phenomenon, name);



    // if(property_set_exists(location, name)) {
    //     throw std::runtime_error("Property set " + name + " already exists");
    // }

    // hdf5::Identifier property_set_location(::create_property_set(location,
    //     name.c_str()), ::close_property_set);

    // if(!property_set_location.is_valid()) {
    //     throw std::runtime_error("Property set " + name +
    //         " cannot be created");
    // }


    // hdf5::Attributes property_set_attributes(property_set_location);

    // property_set_attributes.write<std::string>(
    //     "lue_size_of_item_collection_type",
    //     size_of_item_collection_type_to_string(
    //         configuration.size_of_item_collection_type()));


    // auto const& domain = create_domain(property_set_location,
    //     domain_configuration);

    // auto const& time_configuration = domain_configuration.time();
    // auto const& space_configuration = domain_configuration.space();

    // // TODO
    // assert(configuration.size_of_item_collection_type() ==
    //     SizeOfItemCollectionType::constant_size);

    // switch(time_configuration.type()) {
    //     case TimeDomainType::omnipresent: {
    //         constant_size::time::omnipresent::configure_property_set(
    //             property_set_location, name, space_configuration);
    //         break;
    //     }
    //     case TimeDomainType::shared: {
    //         constant_size::time::shared::configure_property_set(
    //             property_set_location, name, space_configuration);

    //         hid_t const file_type_id = H5T_STD_I32LE;
    //         hid_t const memory_type_id = H5T_NATIVE_INT32;

    //         constant_size::time::shared::configure_time_period_domain(
    //             domain.time_domain().id(), file_type_id, memory_type_id);

    //         break;
    //     }
    // }
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
