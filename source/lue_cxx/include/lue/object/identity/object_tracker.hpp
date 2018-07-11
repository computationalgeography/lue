#pragma once
#include "lue/info/identity/active_id.hpp"
#include "lue/info/identity/active_object_index.hpp"
#include "lue/info/identity/active_set_index.hpp"
#include "lue/info/identity/id.hpp"


namespace lue {

/*!
    @brief      Class for storing object tracking information for all
                object array kinds

    Object array kind | Tracking
    ----------------- | --------
    same_shape::Value | info::ID
    different_shape::Value | info::ID
    same_shape::constant_shape::Value | info::ActiveSetIndex, info::ActiveID
    same_shape::variable_shape::Value | info::ActiveSetIndex, info::ActiveID
    different_shape::constant_shape::Value | info::ActiveSetIndex, info::ActiveObjectIndex, info::ActiveID
    different_shape::variable_shape::Value | -
*/
class ObjectTracker
{

public:

    explicit       ObjectTracker       (hdf5::Group& parent);

                   ObjectTracker       (ObjectTracker const&)=delete;

                   ObjectTracker       (ObjectTracker&&)=default;

                   ~ObjectTracker      ()=default;

    ObjectTracker& operator=           (ObjectTracker const&)=delete;

    ObjectTracker& operator=           (ObjectTracker&&)=default;

    info::ID const& id                 () const;

    info::ID&      id                  ();

    info::ActiveSetIndex const&
                   active_set_index    () const;

    info::ActiveSetIndex&
                   active_set_index    ();

    info::ActiveObjectIndex const&
                   active_object_index () const;

    info::ActiveObjectIndex&
                   active_object_index ();

    info::ActiveID const&
                   active_id           () const;

    info::ActiveID&
                   active_id           ();

private:

    info::ID       _id;

    info::ActiveID _active_id;

    info::ActiveSetIndex _active_set_index;

    info::ActiveObjectIndex _active_object_index;

};


ObjectTracker         create_object_tracker(
                                        hdf5::Group& parent);

}  // namespace lue
