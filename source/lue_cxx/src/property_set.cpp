#include "lue/property_set.hpp"
#include "lue/enum_string_bimap.hpp"
// #include "lue/cxx_api/constant_collection/time/omnipresent/property_set.h"
// #include "lue/cxx_api/constant_collection/time/shared/property_set.h"
// #include "lue/cxx_api/constant_collection/time/shared/time_period_domain.h"
// #include "lue/cxx_api/domain_configuration.h"
// #include "lue/cxx_api/exception.h"
// #include "lue/c_api/domain.h"
// #include "lue/c_api/property_set.h"
// #include "lue/c_api/space_domain.h"
// #include "lue/c_api/time_domain.h"
// #include <cassert>


namespace lue {
namespace {

detail::EnumStringBimap<SizeOfItemCollectionType> const
        size_of_item_collection_type_map = {
    { SizeOfItemCollectionType::constant_collection, "lue_constant_collection" },
    // { SizeOfItemCollectionType::variable_collection, "lue_variable_size" }
};


std::string size_of_item_collection_type_to_string(
    SizeOfItemCollectionType const type)
{
    return size_of_item_collection_type_map.as_string(type);
}


SizeOfItemCollectionType parse_size_of_item_collection_type(
    std::string const& string)
{
    if(!size_of_item_collection_type_map.contains(string)) {
        throw std::runtime_error("Unknown size of item collection type: " +
            string);
    }

    return size_of_item_collection_type_map.as_value(string);
}

}  // Anonymous namespace


PropertySet::Configuration::Configuration(
    SizeOfItemCollectionType const type)

    : _size_of_item_collection_type{type}

{
}


PropertySet::Configuration::Configuration(
    hdf5::Attributes const& attributes)
{
    load(attributes);
}


SizeOfItemCollectionType
    PropertySet::Configuration::size_of_item_collection_type() const
{
    return _size_of_item_collection_type;
}


void PropertySet::Configuration::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        "lue_size_of_item_collection_type",
        size_of_item_collection_type_to_string(_size_of_item_collection_type)
    );
}


void PropertySet::Configuration::load(
    hdf5::Attributes const& attributes)
{
    _size_of_item_collection_type =
        parse_size_of_item_collection_type(
            attributes.read<std::string>(
                "lue_size_of_item_collection_type"));
}


// bool property_set_exists(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     return ::property_set_exists(location, name.c_str());
// }


/*!
    @brief      Construct an instance based on an existing property set
    @param      parent Parent group in dataset of property set named @a name
    @param      name Name of property set to open
    @exception  std::runtime_error In case property set cannot be opened
    @warning    It is assumed that a domain and property set collection
                exist in property set @a name at @a location
*/
PropertySet::PropertySet(
    hdf5::Group const& parent,
    std::string const& name)

    : hdf5::Group(parent, name),
      _configuration(attributes()),
      _domain(*this),
      _properties(*this)

{
    // if(!id().is_valid()) {
    //     throw std::runtime_error("Property set " + name + " cannot be opened");
    // }

    // // Open domain and property collection.
    // assert(domain_exists(id()));
    // _domain = std::make_unique<Domain>(id());
    // assert(_domain->id().is_valid());

    // assert(properties_exists(id()));
    // _properties = std::make_unique<Properties<>>(id());
    // assert(_properties->id().is_valid());
}


// PropertySet::PropertySet(
//     hdf5::Identifier const& id)
// 
//     : hdf5::Group{id},
//       _configuration(attributes()),
//       _domain(this->id()),
//       _properties(this->id())
// 
// {
// }


// PropertySet::PropertySet(
//     hdf5::Identifier&& location)
// 
//     : hdf5::Group(std::forward<hdf5::Identifier>(location))
//       // _configuration{PropertySetConfiguration(
//       //     parse_size_of_item_collection_type(
//       //         attributes().read<std::string>(
//       //             "lue_size_of_item_collection_type")))
//       // }
// 
// {
//     // assert(id().is_valid());
// 
//     // // Open domain and property collection.
//     // assert(domain_exists(id()));
//     // _domain = std::make_unique<Domain>(id());
//     // assert(_domain->id().is_valid());
// 
//     // assert(properties_exists(id()));
//     // _properties = std::make_unique<Properties<>>(id());
//     // assert(_properties->id().is_valid());
// }


PropertySet::PropertySet(
    hdf5::Group&& group)

    : hdf5::Group(std::forward<hdf5::Group>(group)),
      _configuration{attributes()},
      _domain{*this},
      _properties{*this}

{
}


PropertySet::Configuration const& PropertySet::configuration() const
{
    return _configuration;
}


/*!
    @brief      Return the domain
*/
Domain const& PropertySet::domain() const
{
    return _domain;
}


/*!
    @brief      Return the properties
*/
Properties const& PropertySet::properties() const
{
    return _properties;
}


/*!
    @brief      Return the properties
*/
Properties& PropertySet::properties()
{
    return _properties;
}


// /*!
//     @brief      Return domain
// */
// Domain& PropertySet::domain() const
// {
//     assert(_domain);
//     assert(_domain->id().is_valid());
//     return *_domain;
// }
// 
// 
// /*!
//     @brief      Add property to collection
//     @param      name Name of property to add
// */
// Property& PropertySet::add_property(
//     std::string const& name,
//     ValueConfiguration const& value_configuration)
// {
//     assert(_properties->id().is_valid());
//     return properties().add(name, domain().id(), value_configuration);
// }
// 
// 
// /*!
//     @brief      Return property collection
// */
// Properties<>& PropertySet::properties() const
// {
//     assert(_properties);
//     assert(_properties->id().is_valid());
//     return *_properties;
// }



PropertySet create_property_set(
    hdf5::Group const& group,
    std::string const& name,
    PropertySet::Configuration const& configuration,
    Domain::Configuration const& domain_configuration)
{
    auto property_set_group = hdf5::create_group(group, name);

    configuration.save(property_set_group.attributes());
    create_properties(property_set_group);

    create_domain(property_set_group, domain_configuration);

    return PropertySet{std::move(property_set_group)};
}


/*
    @brief      Create a new property set in the dataset
    @param      location Where to create property set
    @param      name Name of property set to create
    @param      domain_configuration Configuration of domain
    @return     Instance refering to the new property set
    @exception  std::runtime_error In case property set @a name already exists
                at @a location
    @exception  std::runtime_error In case property set cannot be created
    @sa         PropertySets::add()
PropertySet create_property_set(
    hdf5::Identifier const& location,
    std::string const& name)
    // PropertySetConfiguration const& configuration,
    // DomainConfiguration const& domain_configuration)
{
    auto property_set = hdf5::create_group(location, name);

    // ...

    return PropertySet(std::move(property_set));



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
    //     SizeOfItemCollectionType::constant_collection);

    // switch(time_configuration.type()) {
    //     case TimeDomainType::omnipresent: {
    //         constant_collection::time::omnipresent::configure_property_set(
    //             property_set_location, name, space_configuration);
    //         break;
    //     }
    //     case TimeDomainType::shared: {
    //         constant_collection::time::shared::configure_property_set(
    //             property_set_location, name, space_configuration);

    //         hid_t const file_type_id = H5T_STD_I32LE;
    //         hid_t const memory_type_id = H5T_NATIVE_INT32;

    //         constant_collection::time::shared::configure_time_period_domain(
    //             domain.time_domain().id(), file_type_id, memory_type_id);

    //         break;
    //     }
    // }

    // return PropertySet(std::move(property_set_location));
}
*/

}  // namespace lue
