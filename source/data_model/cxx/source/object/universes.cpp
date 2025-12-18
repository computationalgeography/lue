#include "lue/object/universes.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model {

    Universes::Universes(hdf5::Group const& parent):

        Collection<Universe>{parent, universes_tag}

    {
    }


    Universes::Universes(Collection<Universe>&& collection):

        Collection<Universe>{std::move(collection)}

    {
    }


    auto Universes::add(std::string const& name) -> Universe&
    {
        return Collection::add(name, create_universe(*this, name));
    }


    auto create_universes(hdf5::Group& parent) -> Universes
    {
        if (collection_exists(parent, universes_tag))
        {
            throw std::runtime_error(
                std::format("Universes collection already exists at {}", parent.id().pathname()));
        }

        return Universes{create_collection<Universe>(parent, universes_tag)};
    }

}  // namespace lue::data_model
