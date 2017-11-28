#include "lue/phenomenon.hpp"


namespace lue {

/*!
    @brief      Construct an instance
    @param      parent Parent group of phenomenon to open
    @param      name Name of instance to open
    @exception  std::runtime_error In case the phenomenon cannot be opened
*/
Phenomenon::Phenomenon(
    hdf5::Group const& parent,
    std::string const& name)

    : hdf5::Group(parent, name),
      _property_sets{*this}

{
}


Phenomenon::Phenomenon(
    hdf5::Group&& group)

    : hdf5::Group{std::forward<hdf5::Group>(group)},
      _property_sets{*this}

{
}


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
    hdf5::Group const& parent,
    std::string const& name)
{
    auto group = hdf5::create_group(parent, name);

    create_property_sets(group);

    return Phenomenon{std::move(group)};
}

} // namespace lue
