#include "lue/object/phenomenon.hpp"


namespace lue {

Phenomenon::Phenomenon(
    hdf5::Group& parent,
    std::string const& name):

    hdf5::Group{parent, name},
    _property_sets{*this}

{
}


Phenomenon::Phenomenon(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _property_sets{*this}

{
}


PropertySet& Phenomenon::add_property_set(
    std::string const& name)
{
    return _property_sets.add(name);
}


PropertySet& Phenomenon::add_property_set(
    std::string const& name,
    TimeConfiguration const& time_configuration,
    SpaceConfiguration const& space_configuration)
{
    return _property_sets.add(name, time_configuration, space_configuration);
}


PropertySets const& Phenomenon::property_sets() const
{
    return _property_sets;
}


Phenomenon create_phenomenon(
    hdf5::Group& parent,
    std::string const& name)
{
    auto group = hdf5::create_group(parent, name);

    create_property_sets(group);

    return Phenomenon{std::move(group)};
}

} // namespace lue
