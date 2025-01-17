#include "lue/object/phenomena.hpp"
#include "lue/core/tag.hpp"


namespace lue {
    namespace data_model {

        Phenomena::Phenomena(hdf5::Group const& parent):

            Collection<Phenomenon>{parent, phenomena_tag}

        {
        }


        Phenomena::Phenomena(Collection<Phenomenon>&& collection):

            Collection<Phenomenon>{std::move(collection)}

        {
        }


        Phenomenon& Phenomena::add(std::string const& name, std::string const& description)
        {
            return Collection::add(name, create_phenomenon(*this, name, description));
        }


        Phenomena create_phenomena(hdf5::Group& parent)
        {
            if (collection_exists(parent, phenomena_tag))
            {
                throw std::runtime_error(
                    fmt::format("Phenomena collection already exists at {}", parent.id().pathname()));
            }

            return Phenomena{create_collection<Phenomenon>(parent, phenomena_tag)};
        }

    }  // namespace data_model
}  // namespace lue
