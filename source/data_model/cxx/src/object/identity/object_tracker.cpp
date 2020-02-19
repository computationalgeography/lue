#include "lue/object/identity/object_tracker.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {

/*!
    @brief      Construct an instance in @a parent

    It is assumed that @a parent contains an object tracker
*/
ObjectTracker::ObjectTracker(
    hdf5::Group& parent):

    hdf5::Group{parent, object_tracker_tag},
    _active_object_id{*this},
    _active_set_index{*this},
    _active_object_index{*this}

{
}


ObjectTracker::ObjectTracker(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _active_object_id{*this},
    _active_set_index{*this},
    _active_object_index{*this}

{
}


/*!
    @brief      Return ActiveID instance for storing IDs of active sets
*/
ActiveObjectID const& ObjectTracker::active_object_id() const
{
    return _active_object_id;
}


/*!
    @brief      Return ActiveID instance for storing IDs of active sets
*/
ActiveObjectID& ObjectTracker::active_object_id()
{
    return _active_object_id;
}


/*!
    @brief      Return ActiveSetIndex instance for storing indices of
                active sets
*/
ActiveSetIndex const& ObjectTracker::active_set_index() const
{
    return _active_set_index;
}


/*!
    @brief      Return ActiveSetIndex instance for storing indices of
                active sets
*/
ActiveSetIndex& ObjectTracker::active_set_index()
{
    return _active_set_index;
}


/*!
    @brief      Return ActiveObjectIndex instance for storing indices of
                values in object arrays of active objects
*/
ActiveObjectIndex const& ObjectTracker::active_object_index() const
{
    return _active_object_index;
}


/*!
    @brief      Return ActiveObjectIndex instance for storing indices of
                values in object arrays of active objects
*/
ActiveObjectIndex& ObjectTracker::active_object_index()
{
    return _active_object_index;
}


/*!
    @brief      Create object tracker in @a parent
*/
ObjectTracker create_object_tracker(
    hdf5::Group& parent)
{
    auto group = hdf5::create_group(parent, object_tracker_tag);

    create_active_object_id(group);
    create_active_set_index(group);
    create_active_object_index(group);

    return ObjectTracker{std::move(group)};
}


void link_object_tracker(
    hdf5::Group& parent,
    ObjectTracker& tracker)
{
    parent.create_soft_link(tracker.id(), object_tracker_tag);
}


bool has_linked_object_tracker(
    hdf5::Group const& parent)
{
    return parent.contains_soft_link(object_tracker_tag);
}

}  // namespace data_model
}  // namespace lue
