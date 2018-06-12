#include "lue/property.hpp"
#include "lue/tag.hpp"


namespace lue {

Property::Property(
    hdf5::Identifier const& id)

    : hdf5::Group(id)

{
}


/*!
    @brief      Construct an instance based on an existing property
    @param      parent Parent group in dataset of property named @a name
    @param      name Name of property to open
    @exception  std::runtime_error In case property cannot be opened
*/
Property::Property(
    hdf5::Group const& parent,
    std::string const& name)

    : hdf5::Group(parent, name)

{
}


Property::Property(
    hdf5::Group&& group)

    : hdf5::Group(std::forward<hdf5::Group>(group))

{
}


ItemConfiguration Property::configuration() const
{
    return ItemConfiguration{
        hdf5::PrimaryDataObject{id(), value_tag}.attributes()};
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
    std::string const& name)
{
    auto group = hdf5::create_group(parent, name);

    return Property{std::move(group)};
}

}  // namespace lue
