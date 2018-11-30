#include "lue/object/identity/object_tracker.hpp"


namespace lue {

/*!
    @brief      Construct an instance in @a parent

    It is assumed that @a parent contains an object tracker
*/
ObjectTracker::ObjectTracker(
    hdf5::Group& parent):

    _active_object_id{parent},
    _active_set_index{parent},
    _active_object_index{parent}

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
    create_active_object_id(parent);
    create_active_set_index(parent);
    create_active_object_index(parent);

    return ObjectTracker{parent};
}


// TODO forward to id/index trackers? or turn the tracker into a group itself?
// void link_object_tracker(
//     hdf5::Group& parent,
//     ObjectTracker& tracker)
// {
//     parent.create_soft_link(tracker.id(), object_tracker_tag);
// }
// 
// 
// bool has_linked_object_tracker(
//     hdf5::Group const& parent)
// {
//     return parent.contains_soft_link(object_tracker_tag);
// }

}  // namespace lue
