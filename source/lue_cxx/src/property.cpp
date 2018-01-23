#include "lue/property.hpp"
#include "lue/tag.hpp"


namespace lue {

/*!
    @brief      Construct an instance based on an existing property
    @param      parent Parent group in dataset of property named @a name
    @param      name Name of property to open
    @exception  std::runtime_error In case property cannot be opened
*/
Property::Property(
    hdf5::Group const& parent,
    std::string const& name)

    : hdf5::Group(parent, name),
      _configuration{attributes()}

{
}


Property::Property(
    hdf5::Group&& group)

    : hdf5::Group(std::forward<hdf5::Group>(group)),
      _configuration{attributes()}

{
}


Configuration const& Property::configuration() const
{
    return _configuration;
}


void Property::discretize_space(
    Property const& property)
{
    create_soft_link(property.id(), space_discretization_tag);
}


bool Property::space_is_discretized() const
{
    return contains_soft_link(space_discretization_tag);
}


Property Property::space_discretization() const
{
    return Property(*this, space_discretization_tag);
}


Property create_property(
    hdf5::Group const& parent,
    std::string const& name,
    Configuration const& configuration)
{
    auto group = hdf5::create_group(parent, name);

    configuration.save(group.attributes());

    return Property{std::move(group)};
}

}  // namespace lue
