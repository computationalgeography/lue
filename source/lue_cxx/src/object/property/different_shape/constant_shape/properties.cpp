#include "lue/object/property/different_shape/constant_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace different_shape {
namespace constant_shape {

Properties::Properties(
    hdf5::Group& parent):

    Collection<Property>{parent, different_shape_constant_shape_tag},
    _active_object_id{*this},
    _active_set_index{*this},
    _active_object_index{*this}

{
}


Properties::Properties(
    Collection<Property>&& collection):

    Collection<Property>{std::forward<Collection<Property>>(collection)},
    _active_object_id{*this},
    _active_set_index{*this},
    _active_object_index{*this}

{
}


Property& Properties::add(
    std::string const& name,
    hdf5::Datatype const& datatype,
    Rank const rank)
{
    return Collection::add(
        name, create_property(*this, name, datatype, rank));
}


Properties create_properties(
    hdf5::Group& parent,
    ActiveObjectID& active_object_id,
    ActiveSetIndex& active_set_index,
    ActiveObjectIndex& active_object_index)
{
    auto collection = create_collection<Property>(
        parent, different_shape_constant_shape_tag);

    collection.create_hard_link(
        active_object_id.id(), active_object_id_tag);
    collection.create_hard_link(
        active_set_index.id(), active_set_index_tag);
    collection.create_hard_link(
        active_object_index.id(), active_object_index_tag);

    return Properties{std::move(collection)};
}

}  // namespace constant_shape
}  // namespace different_shape
}  // namespace lue
