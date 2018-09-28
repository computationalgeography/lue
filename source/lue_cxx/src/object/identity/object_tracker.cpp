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
info::ActiveSetIndex const& ObjectTracker::active_set_index() const
{
    return _active_set_index;
}


/*!
    @brief      Return ActiveSetIndex instance for storing indices of
                active sets
*/
info::ActiveSetIndex& ObjectTracker::active_set_index()
{
    return _active_set_index;
}


/*!
    @brief      Return ActiveObjectIndex instance for storing indices of
                values in object arrays of active objects
*/
info::ActiveObjectIndex const& ObjectTracker::active_object_index() const
{
    return _active_object_index;
}


/*!
    @brief      Return ActiveObjectIndex instance for storing indices of
                values in object arrays of active objects
*/
info::ActiveObjectIndex& ObjectTracker::active_object_index()
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
    info::create_active_set_index(parent);
    info::create_active_object_index(parent);

    return ObjectTracker{parent};
}

}  // namespace lue
