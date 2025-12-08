#include "lue/object/phenomena.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model {

    Phenomena::Phenomena(hdf5::Group const& parent):

        Collection<Phenomenon>{parent, phenomena_tag}

    {
    }


    Phenomena::Phenomena(Collection<Phenomenon>&& collection):

        Collection<Phenomenon>{std::move(collection)}

    {
    }


    auto Phenomena::add(std::string const& name, std::string const& description) -> Phenomenon&
    {
        return Collection::add(name, create_phenomenon(*this, name, description));
    }


    auto create_phenomena(hdf5::Group& parent) -> Phenomena
    {
        if (collection_exists(parent, phenomena_tag))
        {
            throw std::runtime_error(
                std::format("Phenomena collection already exists at {}", parent.id().pathname()));
        }

        return Phenomena{create_collection<Phenomenon>(parent, phenomena_tag)};
    }

}  // namespace lue::data_model
