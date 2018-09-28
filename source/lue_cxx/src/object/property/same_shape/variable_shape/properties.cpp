#include "lue/object/property/same_shape/variable_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace same_shape {
namespace variable_shape {

Properties::Properties(
    hdf5::Group& parent):

    Collection<Property>{parent, same_shape_variable_shape_tag},
    _active_object_id{*this},
    _active_set_index{*this}

{
}


Properties::Properties(
    Collection<Property>&& collection):

    Collection<Property>{std::forward<Collection<Property>>(collection)},
    _active_object_id{*this},
    _active_set_index{*this}

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
    info::ActiveSetIndex& active_set_index)
{
    auto collection = create_collection<Property>(
        parent, same_shape_variable_shape_tag);

    collection.create_hard_link(active_object_id.id(), active_object_id_tag);
    collection.create_hard_link(active_set_index.id(), active_set_index_tag);

    return Properties{std::move(collection)};
}

}  // namespace variable_shape
}  // namespace same_shape
}  // namespace lue
