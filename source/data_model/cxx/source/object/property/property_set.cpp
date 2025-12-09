#include "lue/object/property/property_set.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model {

    /*!
        @brief      Construct an instance based on a @a parent group and a @a name
        @param      parent Parent group of property set
        @param      name Name of group within @a param that represents the property set
    */
    PropertySet::PropertySet(hdf5::Group const& parent, std::string const& name):

        PropertySet{hdf5::Group{parent, name}}

    {
    }


    /*!
        @brief      Construct an instance based on a @a group
        @param      group Group representing the property set
    */
    PropertySet::PropertySet(hdf5::Group&& group):

        hdf5::Group{std::move(group)},
        _object_tracker{*this},
        _time_domain{},
        _space_domain{},
        _properties{*this}

    {
        if (time_domain_exists(*this))
        {
            _time_domain = TimeDomain{*this};
        }

        if (space_domain_exists(*this))
        {
            _space_domain = SpaceDomain{*this};
        }
    }


    /*!
        @brief      Return object tracker
    */
    auto PropertySet::object_tracker() const -> ObjectTracker const&
    {
        return _object_tracker;
    }


    /*!
        @overload
    */
    auto PropertySet::object_tracker() -> ObjectTracker&
    {
        return _object_tracker;
    }


    /*!
        @brief      Return whether the property set owns the object tracker, or shares another property set's
                    object tracker
    */
    auto PropertySet::owns_object_tracker() const -> bool
    {
        return !has_linked_object_tracker(*this);
    }


    /*!
        @brief      Return whether the property set has a time domain

        If the property set does not have a time domain, the information is omnipresent through time.
    */
    auto PropertySet::has_time_domain() const -> bool
    {
        return bool{_time_domain};
    }


    /*!
        @brief      Return whether the property set owns the time domain, or shares another property set's
                    time domain

        In case the property set does not have a time domain false is returned.
    */
    auto PropertySet::owns_time_domain() const -> bool
    {
        return has_time_domain() && !has_linked_time_domain(*this);
    }


    /*!
        @brief      Return whether the property set has a space domain

        If the property set does not have a space domain, the information is omnipresent through space.
    */
    auto PropertySet::has_space_domain() const -> bool
    {
        return bool{_space_domain};
    }


    /*!
        @brief      Return time domain
        @warning    Only call this member function when the property set has a time domain
                    (has_time_domain() returns true)
    */
    auto PropertySet::time_domain() const -> TimeDomain const&
    {
        assert(_time_domain);
        return *_time_domain;  // NOLINT(bugprone-unchecked-optional-access)
    }


    /*!
        @overload
    */
    auto PropertySet::time_domain() -> TimeDomain&
    {
        assert(_time_domain);
        return *_time_domain;  // NOLINT(bugprone-unchecked-optional-access)
    }


    /*!
        @brief      Return space domain
        @warning    Only call this member function when the property set has a space domain
                    (has_space_domain() returns true)
    */
    auto PropertySet::space_domain() const -> SpaceDomain const&
    {
        assert(_space_domain);
        return *_space_domain;  // NOLINT(bugprone-unchecked-optional-access)
    }


    /*!
        @overload
    */
    auto PropertySet::space_domain() -> SpaceDomain&
    {
        assert(_space_domain);
        return *_space_domain;  // NOLINT(bugprone-unchecked-optional-access)
    }


    /*!
        @brief      Return collection of properties
    */
    auto PropertySet::properties() const -> Properties const&
    {
        return _properties;
    }


    /*!
        @overload
    */
    auto PropertySet::properties() -> Properties&
    {
        return _properties;
    }


    auto create_property_set(hdf5::Group& parent, std::string const& name) -> PropertySet
    {
        if (hdf5::group_exists(parent, name))
        {
            throw std::runtime_error(
                std::format("Property-set {} already exists at {}", name, parent.id().pathname()));
        }

        hdf5::Group group{hdf5::create_group(parent, name)};

        create_object_tracker(group);
        create_properties(group);

        return PropertySet{std::move(group)};
    }


    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t const rank) -> PropertySet
    {
        if (hdf5::group_exists(parent, name))
        {
            throw std::runtime_error(
                std::format("Property-set {} already exists at {}", name, parent.id().pathname()));
        }

        hdf5::Group group{hdf5::create_group(parent, name)};

        create_object_tracker(group);
        create_space_domain(group, space_configuration, space_coordinate_datatype, rank);
        create_properties(group);

        return PropertySet{std::move(group)};
    }


    auto create_property_set(hdf5::Group& parent, std::string const& name, TimeDomain& domain) -> PropertySet
    {
        if (hdf5::group_exists(parent, name))
        {
            throw std::runtime_error(
                std::format("Property-set {} already exists at {}", name, parent.id().pathname()));
        }

        hdf5::Group group{hdf5::create_group(parent, name)};

        create_object_tracker(group);
        link_time_domain(group, domain);
        create_properties(group);

        return PropertySet{std::move(group)};
    }


    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        TimeConfiguration const& time_configuration,
        Clock const& clock) -> PropertySet
    {
        if (hdf5::group_exists(parent, name))
        {
            throw std::runtime_error(
                std::format("Property-set {} already exists at {}", name, parent.id().pathname()));
        }

        hdf5::Group group{hdf5::create_group(parent, name)};

        create_object_tracker(group);
        create_time_domain(group, time_configuration, clock);
        create_properties(group);

        return PropertySet{std::move(group)};
    }


    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        TimeConfiguration const& time_configuration,
        Clock const& clock,
        ObjectTracker& object_tracker) -> PropertySet
    {
        if (hdf5::group_exists(parent, name))
        {
            throw std::runtime_error(
                std::format("Property-set {} already exists at {}", name, parent.id().pathname()));
        }

        hdf5::Group group{hdf5::create_group(parent, name)};

        link_object_tracker(group, object_tracker);
        create_time_domain(group, time_configuration, clock);
        create_properties(group);

        return PropertySet{std::move(group)};
    }


    auto create_property_set(
        hdf5::Group& parent, std::string const& name, TimeDomain& domain, ObjectTracker& object_tracker)
        -> PropertySet
    {
        if (hdf5::group_exists(parent, name))
        {
            throw std::runtime_error(
                std::format("Property-set {} already exists at {}", name, parent.id().pathname()));
        }

        hdf5::Group group{hdf5::create_group(parent, name)};

        link_object_tracker(group, object_tracker);
        link_time_domain(group, domain);
        create_properties(group);

        return PropertySet{std::move(group)};
    }


    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        TimeConfiguration const& time_configuration,
        Clock const& clock,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t const rank) -> PropertySet
    {
        if (hdf5::group_exists(parent, name))
        {
            throw std::runtime_error(
                std::format("Property-set {} already exists at {}", name, parent.id().pathname()));
        }

        hdf5::Group group{hdf5::create_group(parent, name)};

        create_object_tracker(group);
        create_time_domain(group, time_configuration, clock);
        create_space_domain(group, space_configuration, space_coordinate_datatype, rank);
        create_properties(group);

        return PropertySet{std::move(group)};
    }


    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        TimeDomain& time_domain,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t rank) -> PropertySet
    {
        if (hdf5::group_exists(parent, name))
        {
            throw std::runtime_error(
                std::format("Property-set {} already exists at {}", name, parent.id().pathname()));
        }

        hdf5::Group group{hdf5::create_group(parent, name)};

        create_object_tracker(group);
        link_time_domain(group, time_domain);
        create_space_domain(group, space_configuration, space_coordinate_datatype, rank);
        create_properties(group);

        return PropertySet{std::move(group)};
    }


    auto create_property_set(
        hdf5::Group& parent,
        std::string const& name,
        TimeDomain& time_domain,
        ObjectTracker& object_tracker,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t rank) -> PropertySet
    {
        if (hdf5::group_exists(parent, name))
        {
            throw std::runtime_error(
                std::format("Property-set {} already exists at {}", name, parent.id().pathname()));
        }

        hdf5::Group group{hdf5::create_group(parent, name)};

        link_object_tracker(group, object_tracker);
        link_time_domain(group, time_domain);
        create_space_domain(group, space_configuration, space_coordinate_datatype, rank);
        create_properties(group);

        return PropertySet{std::move(group)};
    }

}  // namespace lue::data_model
