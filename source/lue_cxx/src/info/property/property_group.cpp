#include "lue/info/property/property_group.hpp"


namespace lue {

PropertyGroup::PropertyGroup(
    hdf5::Group const& parent,
    std::string const& name):

    hdf5::Group{parent, name}

{
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


PropertyGroup create_property_group(
    hdf5::Group& parent,
    std::string const& name)
{
    auto group = hdf5::create_group(parent, name);

    return PropertyGroup{std::move(group)};
}

}  // namespace lue
