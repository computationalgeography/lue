#include "lue/object/phenomenon.hpp"


namespace lue {

Phenomenon::Phenomenon(
    hdf5::Group const& parent,
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
