#include "lue/object/identity/object_tracker.hpp"


namespace lue {

ObjectTracker::ObjectTracker(
    hdf5::Group const& parent):

    _id{parent},
    _active_id{parent},
    _active_set_index{parent},
    _active_object_index{parent}

{
}


info::ID const& ObjectTracker::id() const
{
    return _id;
}


info::ID& ObjectTracker::id()
{
    return _id;
}


info::ActiveID const& ObjectTracker::active_id() const
{
    return _active_id;
}


info::ActiveID& ObjectTracker::active_id()
{
    return _active_id;
}


info::ActiveSetIndex const& ObjectTracker::active_set_index() const
{
    return _active_set_index;
}


info::ActiveSetIndex& ObjectTracker::active_set_index()
{
    return _active_set_index;
}


info::ActiveObjectIndex const& ObjectTracker::active_object_index() const
{
    return _active_object_index;
}


info::ActiveObjectIndex& ObjectTracker::active_object_index()
{
    return _active_object_index;
}


ObjectTracker create_object_tracker(
    hdf5::Group& parent)
{
    info::create_id(parent);
    info::create_active_id(parent);
    info::create_active_set_index(parent);
    info::create_active_object_index(parent);

    return ObjectTracker{parent};
}

}  // namespace lue
