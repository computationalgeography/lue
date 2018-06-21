#include "lue/object/property/same_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace same_shape {

Properties::Properties(
    hdf5::Group const& parent):

    Collection<Property>{parent, same_shape_tag},
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
    auto collection = create_collection<Property>(parent, same_shape_tag);

    collection.create_hard_link(id.id(), id_tag);

    return Properties{std::move(collection)};
}

}  // namespace same_shape
}  // namespace lue
