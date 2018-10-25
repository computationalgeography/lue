#include "lue/object/phenomena.hpp"
#include "lue/core/tag.hpp"


namespace lue {

Phenomena::Phenomena(
    hdf5::Group& parent):

    Collection<Phenomenon>{parent, phenomena_tag}

{
}


Phenomena::Phenomena(
    Collection<Phenomenon>&& collection):

    Collection<Phenomenon>{std::forward<Collection<Phenomenon>>(collection)}

{
}


Phenomenon& Phenomena::add(
    std::string const& name)
{
    return Collection::add(name, create_phenomenon(*this, name));
}


Phenomena create_phenomena(
    hdf5::Group& parent)
{
    if(collection_exists(parent, phenomena_tag)) {
        throw std::runtime_error(fmt::format(
            "Phenomena collection already exists at {}",
            parent.id().pathname()));
    }

    auto collection = create_collection<Phenomenon>(parent, phenomena_tag);

    return Phenomena{std::move(collection)};
}

} // namespace lue
