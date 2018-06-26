#include "lue/object/universe.hpp"


namespace lue {

Universe::Universe(
    hdf5::Group const& parent,
    std::string const& name):

    hdf5::Group{parent, name},
    _phenomena{*this}

{
}


Universe::Universe(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _phenomena{*this}

{
}


Phenomena const& Universe::phenomena() const
{
    return _phenomena;
}


Universe create_universe(
    hdf5::Group& parent,
    std::string const& name)
{
    auto group = hdf5::create_group(parent, name);

    create_property_sets(group);

    return Universe{std::move(group)};
}

} // namespace lue
