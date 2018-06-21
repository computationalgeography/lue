#include "lue/object/property/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {

Properties::Properties(
    hdf5::Group const& parent):

    hdf5::Group{parent, properties_tag},
    _id{*this},
    _active_set_index{*this},
    _active_object_index{*this},
    _active_id{*this}

{
}


Properties::Properties(
    hdf5::Group&& group,
    info::ID&& id,
    info::ActiveSetIndex&& active_set_index,
    info::ActiveObjectIndex&& active_object_index,
    info::ActiveID&& active_id):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _id{std::forward<info::ID>(id)},
    _active_set_index{std::forward<info::ActiveSetIndex>(active_set_index)},
    _active_object_index{std::forward<info::ActiveObjectIndex>(active_object_index)},
    _active_id{std::forward<info::ActiveID>(active_id)}

{
}


info::ID const& Properties::id() const
{
    return _id;
}


info::ActiveSetIndex const& Properties::active_set_index() const
{
    return _active_set_index;
}


info::ActiveObjectIndex const& Properties::active_object_index() const
{
    return _active_object_index;
}


info::ActiveID const& Properties::active_id() const
{
    return _active_id;
}


Properties create_properties(
    hdf5::Group& parent)
{
    auto group = hdf5::create_group(parent, properties_tag);
    auto id = info::create_id(parent);
    auto active_set_index = info::create_active_set_index(parent);
    auto active_object_index = info::create_active_object_index(parent);
    auto active_id = info::create_active_id(parent);

    return Properties{
        std::move(group), std::move(id), std::move(active_set_index),
        std::move(active_object_index), std::move(active_id)};
}

}  // namespace lue
