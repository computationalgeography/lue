#include "lue/property_set.hpp"


namespace lue {

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

    : hdf5::Group{parent, name},
      _configuration{attributes()},
      _domain{*this},
      _properties{*this}

{
}


PropertySet::PropertySet(
    hdf5::Group&& group)

    : hdf5::Group{std::forward<hdf5::Group>(group)},
      _configuration{attributes()},
      _domain{*this},
      _properties{*this}

{
}


PropertySetConfiguration const& PropertySet::configuration() const
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
    @brief      Return the domain
*/
Domain& PropertySet::domain()
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


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    PropertySetConfiguration const& configuration)
{
    auto group = hdf5::create_group(parent, name);

    configuration.save(group.attributes());
    create_properties(group);

    create_domain(group);

    return PropertySet{std::move(group)};
}

}  // namespace lue
