#include "lue/info/property/property_group.hpp"
#include "lue/core/aspect.hpp"
#include <cassert>


namespace lue {

PropertyGroup::PropertyGroup(
    hdf5::Group& parent,
    std::string const& name):

    hdf5::Group{parent, name}

{
    assert(this->name() == name);
}


PropertyGroup::PropertyGroup(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)}

{
}


std::string PropertyGroup::name() const
{
    return id().name();
}


bool PropertyGroup::space_is_discretized() const
{
    return attributes().exists(space_discretization_tag);
}


SpaceDiscretization PropertyGroup::space_discretization_type() const
{
    assert(space_is_discretized());

    return Aspect<SpaceDiscretization>(attributes()).value();
}


void PropertyGroup::set_space_discretisation(
    SpaceDiscretization type,
    PropertyGroup& property)
{
    Aspect<SpaceDiscretization>(type).save(attributes());
    create_soft_link(property.id(), space_discretization_property_tag);
}


PropertyGroup PropertyGroup::space_discretization_property()
{
    return PropertyGroup(*this, space_discretization_property_tag);
}


PropertyGroup create_property_group(
    hdf5::Group& parent,
    std::string const& name)
{
    auto group = hdf5::create_group(parent, name);

    return PropertyGroup{std::move(group)};
}

}  // namespace lue
