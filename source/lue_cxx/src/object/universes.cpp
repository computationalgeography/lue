#include "lue/object/universes.hpp"
#include "lue/core/tag.hpp"


namespace lue {

Universes::Universes(
    hdf5::Group& parent):

    Collection<Universe>{parent, universes_tag}

{
}


Universes::Universes(
    Collection<Universe>&& collection):

    Collection<Universe>{std::forward<Collection<Universe>>(collection)}

{
}


Universe& Universes::add(
    std::string const& name)
{
    return Collection::add(name, create_universe(*this, name));
}


Universes create_universes(
    hdf5::Group& parent)
{
    auto collection = create_collection<Universe>(parent, universes_tag);

    return Universes{std::move(collection)};
}

} // namespace lue
