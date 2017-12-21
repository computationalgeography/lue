#include "lue/constant_size/time/omnipresent/property_set.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

/*!
    @brief      Create instance based on a identifier
    @param      id Identifier of a property set
*/
PropertySet::PropertySet(
    hdf5::Identifier const& id)

    : constant_size::PropertySet(id),
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


/*!
    @brief      TODO
    @param      .
    @return     .
    @exception  .
*/
PropertySet create_property_set(
    Phenomenon& phenomenon,
    std::string const& name)
    // SpaceDomain::Configuration const& space_domain_configuration)
{
    auto& property_set = phenomenon.property_sets().add(name,
        constant_size::create_property_set(
            phenomenon.property_sets(), name,
            Domain::Configuration(
                Domain::Configuration::DomainType::omnipresent)
    ));

    // omnipresent::create_space_domain(
    //     property_set.domain(), space_domain_configuration);



    // auto domain = omnipresent::create_domain(
    //     property_set.id(), Domain::Configuration(TimeDomainType::omnipresent));

    return PropertySet{property_set.id()};




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


/*!
    @brief      TODO
    @param      .
    @return     .
    @exception  .
*/
PropertySet create_property_set(
    Phenomenon& phenomenon,
    std::string const& name,
    PropertySet::Ids const& ids)
{
    auto& property_set = phenomenon.property_sets().add(name,
        constant_size::create_property_set(
            phenomenon.property_sets(), name, ids,
            Domain::Configuration(
                Domain::Configuration::DomainType::omnipresent)
    ));

    return PropertySet{property_set.id()};
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
