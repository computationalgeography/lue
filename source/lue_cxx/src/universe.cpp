#include "lue/universe.hpp"


namespace lue {

// bool universe_exists(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     return ::universe_exists(location, name.c_str()) > 0;
// }


Universe::Universe(
    hdf5::Identifier const& location,
    std::string const& name)

    : hdf5::Group(location, name),
      _phenomena(id())

{
}


Universe::Universe(
    hdf5::Identifier&& location)

    : hdf5::Group(std::forward<hdf5::Identifier>(location)),
      _phenomena(id())

{
}


Universe::Universe(
    hdf5::Group&& group)

    : hdf5::Group(std::forward<hdf5::Group>(group)),
      _phenomena(id())

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
    @param      Location Id of location of file or group to add
                universe to
    @param      Name name of universe
    @exception  std::runtime_error In case a universe with the name
                passed in already exists
    @exception  std::runtime_error In case the universe cannot be
                created
*/
Universe create_universe(
    hdf5::Identifier const& location,
    std::string const& name)
{
    auto universe = hdf5::create_group(location, name);

    create_phenomena(universe.id());

    return Universe(std::move(universe));
}

} // namespace lue
