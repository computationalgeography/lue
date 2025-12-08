#pragma once
#include "lue/info/identity/active_object_id.hpp"
#include "lue/info/identity/active_object_index.hpp"
#include "lue/info/identity/active_set_index.hpp"


namespace lue::data_model {

    /*!
        @brief      Class for storing object tracking information for all temporal object array kinds

        Object array kind | Tracking
        ----------------- | --------
        same_shape::constant_shape::Value | ActiveSetIndex, ActiveObjectID
        same_shape::variable_shape::Value | ActiveSetIndex, ActiveObjectID
        different_shape::constant_shape::Value | ActiveSetIndex, ActiveObjectIndex, ActiveObjectID
        different_shape::variable_shape::Value | -
    */
    class LUE_DATA_MODEL_EXPORT ObjectTracker: public hdf5::Group
    {

        public:

            explicit ObjectTracker(hdf5::Group const& parent);

            explicit ObjectTracker(hdf5::Group&& group);

            ObjectTracker(ObjectTracker const& other) = default;

            ObjectTracker(ObjectTracker&& other) = default;

            ~ObjectTracker() override = default;

            auto operator=(ObjectTracker const& other) -> ObjectTracker& = default;

            auto operator=(ObjectTracker&& other) -> ObjectTracker& = default;

            auto active_set_index() const -> ActiveSetIndex const&;

            auto active_set_index() -> ActiveSetIndex&;

            auto active_object_index() const -> ActiveObjectIndex const&;

            auto active_object_index() -> ActiveObjectIndex&;

            auto active_object_id() const -> ActiveObjectID const&;

            auto active_object_id() -> ActiveObjectID&;

        private:

            ActiveObjectID _active_object_id;

            ActiveSetIndex _active_set_index;

            ActiveObjectIndex _active_object_index;
    };


    auto create_object_tracker(hdf5::Group& parent) -> ObjectTracker;

    void link_object_tracker(hdf5::Group& parent, ObjectTracker& object_tracker);

    auto has_linked_object_tracker(hdf5::Group const& parent) -> bool;

}  // namespace lue::data_model
