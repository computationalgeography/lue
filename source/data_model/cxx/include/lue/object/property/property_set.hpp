#pragma once
#include "lue/object/identity/object_tracker.hpp"
#include "lue/object/property/properties.hpp"
#include "lue/object/space_domain.hpp"
#include "lue/object/time_domain.hpp"
#include <optional>


namespace lue::data_model {

    /*!
        @brief      Class representing a set of properties sharing a time
                    and space domain
    */
    class LUE_DATA_MODEL_EXPORT PropertySet: public hdf5::Group
    {

        public:

            PropertySet(hdf5::Group const& parent, std::string const& name);

            explicit PropertySet(hdf5::Group&& group);

            PropertySet(PropertySet const& other) = default;

            PropertySet(PropertySet&& other) = default;

            ~PropertySet() override = default;

            auto operator=(PropertySet const& other) -> PropertySet& = default;

            auto operator=(PropertySet&& other) -> PropertySet& = default;

            auto object_tracker() const -> ObjectTracker const&;

            auto object_tracker() -> ObjectTracker&;

            auto owns_object_tracker() const -> bool;

            auto has_time_domain() const -> bool;

            auto owns_time_domain() const -> bool;

            auto has_space_domain() const -> bool;

            auto time_domain() const -> TimeDomain const&;

            auto time_domain() -> TimeDomain&;

            auto space_domain() const -> SpaceDomain const&;

            auto space_domain() -> SpaceDomain&;

            auto properties() const -> Properties const&;

            auto properties() -> Properties&;

        private:

            ObjectTracker _object_tracker;

            std::optional<TimeDomain> _time_domain;

            std::optional<SpaceDomain> _space_domain;

            Properties _properties;
    };


    auto create_property_set(hdf5::Group& parent, std::string const& name) -> PropertySet;

    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t rank) -> PropertySet;

    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        TimeConfiguration const& time_configuration,
        Clock const& clock) -> PropertySet;

    auto create_property_set(hdf5::Group& parent, std::string const& name, TimeDomain& domain) -> PropertySet;

    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        TimeConfiguration const& time_configuration,
        Clock const& clock,
        ObjectTracker& object_tracker) -> PropertySet;

    auto create_property_set(
        hdf5::Group& parent, std::string const& name, TimeDomain& domain, ObjectTracker& object_tracker)
        -> PropertySet;

    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        TimeConfiguration const& time_configuration,
        Clock const& clock,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t rank) -> PropertySet;

    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        TimeDomain& time_domain,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t rank) -> PropertySet;

    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        TimeDomain& time_domain,
        ObjectTracker& object_tracker,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t rank) -> PropertySet;

    // PropertySet        create_property_set (hdf5::Group& parent,
    //                                         std::string const& name,
    //                                         TimeDomain& time_domain,
    //                                         SpaceConfiguration const&
    //                                             space_configuration);

}  // namespace lue::data_model
