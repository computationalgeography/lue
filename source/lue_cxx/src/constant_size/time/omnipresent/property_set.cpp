#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/tag.hpp"


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
      _ids(this->id(), ids_tag, H5T_NATIVE_HSIZE)

{
}


// PropertySet::PropertySet(
//     constant_size::PropertySet&& property_set)
// 
//     : constant_size::PropertySet(std::forward<constant_size::PropertySet>(
//         property_set)),
//       _domain(id()),
//       _ids(id(), ids_tag, H5T_NATIVE_HSIZE)
// 
// {
// }


same_shape::Value const& PropertySet::ids() const
{
    return _ids;
}


same_shape::Value& PropertySet::ids()
{
    return _ids;
}


Domain const& PropertySet::domain() const
{
    return _domain;
}


Domain& PropertySet::domain()
{
    return _domain;
}


same_shape::Value& PropertySet::reserve(
    hsize_t const nr_items)
{
    _ids.reserve(nr_items);

    return _ids;
}


PropertySet create_property_set(
    PropertySets& property_sets,
    std::string const& name)
    // SpaceDomain::Configuration const& space_domain_configuration)
{
    auto& property_set = property_sets.add(name,
        constant_size::create_property_set(
            property_sets, name,
            Domain::Configuration(
                Domain::Configuration::DomainType::omnipresent)
    ));

    // omnipresent::create_space_domain(
    //     property_set.domain(), space_domain_configuration);



    // auto domain = omnipresent::create_domain(
    //     property_set.id(), Domain::Configuration(TimeDomainType::omnipresent));

    same_shape::create_value(
        property_set.id(), ids_tag, H5T_STD_U64LE, H5T_NATIVE_HSIZE);

    return PropertySet(property_set.id());




    // return PropertySet(std::move(property_set));

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


PropertySet create_property_set(
    PropertySets& property_sets,
    std::string const& name,
    same_shape::Value const& ids)
{
    auto& property_set = property_sets.add(name,
        constant_size::create_property_set(
            property_sets, name,
            Domain::Configuration(
                Domain::Configuration::DomainType::omnipresent)
    ));

    // TODO assert
    // same_shape::create_value(property_set.id(), ids_tag,
    //     H5T_STD_U64LE, H5T_NATIVE_HSIZE);

    property_set.create_hard_link(ids.id(), ids_tag);

    return PropertySet(property_set.id());
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
