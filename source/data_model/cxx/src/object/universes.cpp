#include "lue/object/universes.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {

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
    if(collection_exists(parent, universes_tag)) {
        throw std::runtime_error(fmt::format(
            "Universes collection already exists at {}",
            parent.id().pathname()));
    }

    return Universes{create_collection<Universe>(parent, universes_tag)};
}

}  // namespace data_model
}  // namespace lue
