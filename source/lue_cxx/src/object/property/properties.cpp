#include "lue/object/property/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {

Properties::Properties(
    hdf5::Group const& parent):

    hdf5::Group{parent, properties_tag},
    _id{*this},
    _active_id{*this},
    _active_set_index{*this},
    _active_object_index{*this},
    _same_shape_properties{*this},
    _same_shape_constant_shape_properties{*this},
    _same_shape_variable_shape_properties{*this},
    _different_shape_properties{*this}

{
}


Properties::Properties(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _id{*this},
    _active_id{*this},
    _active_set_index{*this},
    _active_object_index{*this},
    _same_shape_properties{*this},
    _same_shape_constant_shape_properties{*this},
    _same_shape_variable_shape_properties{*this},
    _different_shape_properties{*this}

{
}


info::ID const& Properties::id() const
{
    return _id;
}


info::ActiveID const& Properties::active_id() const
{
    return _active_id;
}


info::ActiveSetIndex const& Properties::active_set_index() const
{
    return _active_set_index;
}


info::ActiveObjectIndex const& Properties::active_object_index() const
{
    return _active_object_index;
}


Properties create_properties(
    hdf5::Group& parent)
{
    // Create all stuff. Pass the object ID tracking stuff to the
    // specialized Properties classes. They will hard-link to it. That
    // way this information can be shared.
    auto group = hdf5::create_group(parent, properties_tag);
    auto id = info::create_id(group);
    auto active_id = info::create_active_id(group);
    auto active_set_index = info::create_active_set_index(group);
    info::create_active_object_index(group);

    same_shape::create_properties(group, id);
    same_shape::constant_shape::create_properties(
        group, active_id, active_set_index);
    same_shape::variable_shape::create_properties(
        group, active_id, active_set_index);
    different_shape::create_properties(group, id);

    return Properties{std::move(group)};
}

}  // namespace lue
