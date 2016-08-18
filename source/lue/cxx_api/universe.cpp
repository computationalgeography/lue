#include "lue/cxx_api/universe.h"
#include "lue/c_api/universe.h"
#include <cassert>
#include <stdexcept>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
bool universe_exists(
    hdf5::Identifier const& location,
    std::string const& name)
{
    return ::universe_exists(location, name.c_str()) > 0;
}


Universe::Universe(
    hdf5::Identifier const& location,
    std::string const& name)

    : hdf5::Group(hdf5::Identifier(::open_universe(location, name.c_str()),
        ::close_universe))

{
    if(!id().is_valid()) {
        throw std::runtime_error("Universe " + name + " cannot be opened");
    }

    // Open phenomenon collection.
    assert(phenomena_exists(id()));
    _phenomena = std::make_unique<Phenomena>(id());
}


Universe::Universe(
    hdf5::Identifier&& location)

    : hdf5::Group(std::forward<hdf5::Identifier>(location))

{
    assert(id().is_valid());

    // Open phenomenon collection.
    assert(phenomena_exists(id()));
    _phenomena = std::make_unique<Phenomena>(id());
}


Phenomenon& Universe::add_phenomenon(
    std::string const& name)
{
    return phenomena().add(name);
}


Phenomena& Universe::phenomena() const
{
    assert(_phenomena);
    return *_phenomena;
}


Universe create_universe(
    hdf5::Identifier const& location,
    std::string const& name)
{
    if(universe_exists(location, name)) {
        throw std::runtime_error("Universe " + name + " already exists");
    }

    hdf5::Identifier universe_location(::create_universe(location,
        name.c_str()), ::close_universe);

    if(!universe_location.is_valid()) {
        throw std::runtime_error("Universe " + name + " cannot be created");
    }

    return Universe(std::move(universe_location));
}

} // namespace lue
