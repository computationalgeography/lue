#include "lue/phenomenon.hpp"


namespace lue {

// bool phenomenon_exists(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     return ::phenomenon_exists(location, name.c_str()) > 0;
// }


/*!
    @brief      Construct an instance
    @param      location Location of group of phenomenon to open
    @param      name Name of instance to open
    @exception  std::runtime_error In case the phenomenon cannot be opened
*/
Phenomenon::Phenomenon(
    hdf5::Identifier const& location,
    std::string const& name)

    : hdf5::Group(location, name.c_str()),
      _property_sets(id())

{
}


/*!
    @brief      Construct an instance
    @param      location Location of open phenomenon
*/
Phenomenon::Phenomenon(
    hdf5::Identifier&& location)

    : hdf5::Group(std::forward<hdf5::Identifier>(location)),
      _property_sets(id())

{
}


Phenomenon::Phenomenon(
    hdf5::Group&& group)

    : hdf5::Group(std::forward<hdf5::Group>(group)),
      _property_sets(id())

{
}


// PropertySet& Phenomenon::add_property_set(
//     std::string const& name)
// {
//     return _property_sets.add(name);
// }


// PropertySet& Phenomenon::add_property_set(
//     std::string const& name,
//     PropertySetConfiguration const& configuration,
//     DomainConfiguration const& domain_configuration)
// {
//     return _property_sets.add(name, configuration, domain_configuration);
// }


/*!
    @brief      Return the property sets
*/
PropertySets const& Phenomenon::property_sets() const
{
    return _property_sets;
}


/*!
    @brief      Return the property sets
*/
PropertySets& Phenomenon::property_sets()
{
    return _property_sets;
}


/*!
    @brief      Add new phenomenon
    @param      Location Id of location of file or group to add
                phenomenon to
    @param      Name name of phenomenon
    @exception  std::runtime_error In case a phenomenon with the name
                passed in already exists
    @exception  std::runtime_error In case the phenomenon cannot be
                created
*/
Phenomenon create_phenomenon(
    hdf5::Identifier const& location,
    std::string const& name)
{
    auto phenomenon = hdf5::create_group(location, name);

    create_property_sets(phenomenon.id());

    return Phenomenon(std::move(phenomenon));
}

} // namespace lue
