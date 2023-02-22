#pragma once
#include "lue/object/identity/object_tracker.hpp"
#include "lue/object/property/properties.hpp"
#include "lue/object/space_domain.hpp"
#include "lue/object/time_domain.hpp"
// #include <memory>
#include <optional>


namespace lue {
    namespace data_model {

        /*!
            @brief      Class representing a set of properties sharing a time
                        and space domain
        */
        class PropertySet: public hdf5::Group
        {

            public:

                PropertySet(hdf5::Group const& parent, std::string const& name);

                explicit PropertySet(hdf5::Group&& group);

                PropertySet(PropertySet const&) = default;

                PropertySet(PropertySet&&) = default;

                ~PropertySet() override = default;

                PropertySet& operator=(PropertySet const&) = default;

                PropertySet& operator=(PropertySet&&) = default;

                ObjectTracker const& object_tracker() const;

                ObjectTracker& object_tracker();

                bool owns_object_tracker() const;

                bool has_time_domain() const;

                bool owns_time_domain() const;

                bool has_space_domain() const;

                TimeDomain const& time_domain() const;

                TimeDomain& time_domain();

                SpaceDomain const& space_domain() const;

                SpaceDomain& space_domain();

                Properties const& properties() const;

                Properties& properties();

            private:

                ObjectTracker _object_tracker;

                std::optional<TimeDomain> _time_domain;

                std::optional<SpaceDomain> _space_domain;

                Properties _properties;
        };


        PropertySet create_property_set(hdf5::Group& parent, std::string const& name);

        PropertySet create_property_set(
            hdf5::Group& parent,
            std::string const& name,
            SpaceConfiguration const& space_configuration,
            hdf5::Datatype const& space_coordinate_datatype,
            std::size_t rank);

        PropertySet create_property_set(
            hdf5::Group& parent,
            std::string const& name,
            TimeConfiguration const& time_configuration,
            Clock const& clock);

        PropertySet create_property_set(hdf5::Group& parent, std::string const& name, TimeDomain& domain);

        PropertySet create_property_set(
            hdf5::Group& parent,
            std::string const& name,
            TimeConfiguration const& time_configuration,
            Clock const& clock,
            ObjectTracker& object_tracker);

        PropertySet create_property_set(
            hdf5::Group& parent, std::string const& name, TimeDomain& domain, ObjectTracker& object_tracker);

        PropertySet create_property_set(
            hdf5::Group& parent,
            std::string const& name,
            TimeConfiguration const& time_configuration,
            Clock const& clock,
            SpaceConfiguration const& space_configuration,
            hdf5::Datatype const& space_coordinate_datatype,
            std::size_t rank);

        PropertySet create_property_set(
            hdf5::Group& parent,
            std::string const& name,
            TimeDomain& time_domain,
            SpaceConfiguration const& space_configuration,
            hdf5::Datatype const& space_coordinate_datatype,
            std::size_t rank);

        PropertySet create_property_set(
            hdf5::Group& parent,
            std::string const& name,
            TimeDomain& time_domain,
            ObjectTracker& object_tracker,
            SpaceConfiguration const& space_configuration,
            hdf5::Datatype const& space_coordinate_datatype,
            std::size_t rank);

        // PropertySet        create_property_set (hdf5::Group& parent,
        //                                         std::string const& name,
        //                                         TimeDomain& time_domain,
        //                                         SpaceConfiguration const&
        //                                             space_configuration);

    }  // namespace data_model
}  // namespace lue
