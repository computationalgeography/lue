#include "lue/object/property/different_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace different_shape {

Properties::Properties(
    hdf5::Group& parent):

    Collection<Property>{parent, different_shape_tag},
    _id{*this}

{
}


Properties::Properties(
    Collection<Property>&& collection):

    Collection<Property>{std::forward<Collection<Property>>(collection)},
    _id{*this}

{
}


Properties create_properties(
    hdf5::Group& parent,
    info::ID& id)
{
    auto collection = create_collection<Property>(parent, different_shape_tag);

    collection.create_hard_link(id.id(), id_tag);

    return Properties{std::move(collection)};
}

}  // namespace different_shape
}  // namespace lue
