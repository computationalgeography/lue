#include "lue/object/identity/object_tracker.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model {

    /*!
        @brief      Construct an instance in @a parent

        It is assumed that @a parent contains an object tracker
    */
    ObjectTracker::ObjectTracker(hdf5::Group const& parent):

        hdf5::Group{parent, object_tracker_tag},
        _active_object_id{*this},
        _active_set_index{*this},
        _active_object_index{*this}

    {
    }


    ObjectTracker::ObjectTracker(hdf5::Group&& group):

        hdf5::Group{std::move(group)},
        _active_object_id{*this},
        _active_set_index{*this},
        _active_object_index{*this}

    {
    }


    /*!
        @brief      Return ActiveID instance for storing IDs of active sets
    */
    auto ObjectTracker::active_object_id() const -> ActiveObjectID const&
    {
        return _active_object_id;
    }


    /*!
        @brief      Return ActiveID instance for storing IDs of active sets
    */
    auto ObjectTracker::active_object_id() -> ActiveObjectID&
    {
        return _active_object_id;
    }


    /*!
        @brief      Return ActiveSetIndex instance for storing indices of active sets
    */
    auto ObjectTracker::active_set_index() const -> ActiveSetIndex const&
    {
        return _active_set_index;
    }


    /*!
        @brief      Return ActiveSetIndex instance for storing indices of active sets
    */
    auto ObjectTracker::active_set_index() -> ActiveSetIndex&
    {
        return _active_set_index;
    }


    /*!
        @brief      Return ActiveObjectIndex instance for storing indices of values in object arrays of
                    active objects
    */
    auto ObjectTracker::active_object_index() const -> ActiveObjectIndex const&
    {
        return _active_object_index;
    }


    /*!
        @brief      Return ActiveObjectIndex instance for storing indices of values in object arrays of
                    active objects
    */
    auto ObjectTracker::active_object_index() -> ActiveObjectIndex&
    {
        return _active_object_index;
    }


    /*!
        @brief      Create object tracker in @a parent
    */
    auto create_object_tracker(hdf5::Group& parent) -> ObjectTracker
    {
        hdf5::Group group{hdf5::create_group(parent, object_tracker_tag)};

        create_active_object_id(group);
        create_active_set_index(group);
        create_active_object_index(group);

        return ObjectTracker{std::move(group)};
    }


    void link_object_tracker(hdf5::Group& parent, ObjectTracker& tracker)
    {
        parent.create_soft_link(tracker.id(), object_tracker_tag);
    }


    auto has_linked_object_tracker(hdf5::Group const& parent) -> bool
    {
        return parent.contains_soft_link(object_tracker_tag);
    }

}  // namespace lue::data_model
