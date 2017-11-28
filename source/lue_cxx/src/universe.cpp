#include "lue/universe.hpp"


namespace lue {

Universe::Universe(
    hdf5::Group const& parent,
    std::string const& name)

    : hdf5::Group(parent, name),
      _phenomena{*this}

{
}


Universe::Universe(
    hdf5::Group&& group)

    : hdf5::Group(std::forward<hdf5::Group>(group)),
      _phenomena{*this}

{
}


Phenomenon& Universe::add_phenomenon(
    std::string const& name)
{
    return _phenomena.add(name);
}


Phenomena const& Universe::phenomena() const
{
    return _phenomena;
}


Phenomena& Universe::phenomena()
{
    return _phenomena;
}


/*!
    @brief      Add new universe
    @param      parent Group to create collection in
    @param      Name name of universe
    @exception  std::runtime_error In case a universe with the name
                passed in already exists
    @exception  std::runtime_error In case the universe cannot be
                created
*/
Universe create_universe(
    hdf5::Group const& parent,
    std::string const& name)
{
    auto group = hdf5::create_group(parent, name);

    create_phenomena(group);

    return Universe{std::move(group)};
}

} // namespace lue
