#include "lue/object/property/property_set.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {

/*!
    @brief      Construct an instance based on a @a parent group and a
                @a name
    @param      parent Parent group of property set
    @param      name Name of group within @a param that represents the
                property set
*/
PropertySet::PropertySet(
    hdf5::Group& parent,
    std::string const& name):

    PropertySet{hdf5::Group{parent, name}}

{
}


/*!
    @brief      Construct an instance based on a @a group
    @param      group Group representing the property set
*/
PropertySet::PropertySet(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _object_tracker{*this},
    _time_domain{},
    _space_domain{},
    _properties{*this}

{
    if(time_domain_exists(*this)) {
        _time_domain = std::make_unique<TimeDomain>(*this);
    }

    if(space_domain_exists(*this)) {
        _space_domain = std::make_unique<SpaceDomain>(*this);
    }
}


/*!
    @brief      Return object tracker
*/
ObjectTracker const& PropertySet::object_tracker() const
{
    return _object_tracker;
}


/*!
    @overload
*/
ObjectTracker& PropertySet::object_tracker()
{
    return _object_tracker;
}


/*!
    @brief      Return whether the property set owns the object tracker, or
                shares another property set's object tracker
*/
bool PropertySet::owns_object_tracker() const
{
    return !has_linked_object_tracker(*this);
}


/*!
    @brief      Return whether the property set has a time domain

    If the property set does not have a time domain, the information is
    omnipresent through time.
*/
bool PropertySet::has_time_domain() const
{
    return _time_domain ? true : false;
}


/*!
    @brief      Return whether the property set owns the time domain, or
                shares another property set's time domain

    In case the property set does not have a time domain false is returned.
*/
bool PropertySet::owns_time_domain() const
{
    return has_time_domain() && !has_linked_time_domain(*this);
}


/*!
    @brief      Return whether the property set has a space domain

    If the property set does not have a space domain, the information is
    omnipresent through space.
*/
bool PropertySet::has_space_domain() const
{
    return _space_domain ? true : false;
}


/*!
    @brief      Return time domain
    @warning    Only call this member function when the property set
                has a time domain (has_time_domain() returns true)
*/
TimeDomain const& PropertySet::time_domain() const
{
    assert(_time_domain);
    return *_time_domain;
}


/*!
    @overload
*/
TimeDomain& PropertySet::time_domain()
{
    assert(_time_domain);
    return *_time_domain;
}


/*!
    @brief      Return space domain
    @warning    Only call this member function when the property set
                has a space domain (has_space_domain() returns true)
*/
SpaceDomain const& PropertySet::space_domain() const
{
    assert(_space_domain);
    return *_space_domain;
}


/*!
    @overload
*/
SpaceDomain& PropertySet::space_domain()
{
    assert(_space_domain);
    return *_space_domain;
}


/*!
    @brief      Return collection of properties
*/
Properties const& PropertySet::properties() const
{
    return _properties;
}


/*!
    @overload
*/
Properties& PropertySet::properties()
{
    return _properties;
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name)
{
    if(hdf5::group_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Property-set {} already exists at {}",
            name, parent.id().pathname()));
    }

    auto group = hdf5::create_group(parent, name);

    create_object_tracker(group);
    create_properties(group);

    return PropertySet{std::move(group)};
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    SpaceConfiguration const& space_configuration,
    hdf5::Datatype const& space_coordinate_datatype,
    std::size_t const rank)
{
    if(hdf5::group_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Property-set {} already exists at {}",
            name, parent.id().pathname()));
    }

    auto group = hdf5::create_group(parent, name);

    create_object_tracker(group);
    create_space_domain(
        group, space_configuration, space_coordinate_datatype, rank);
    create_properties(group);

    return PropertySet{std::move(group)};
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    TimeDomain& domain)
{
    if(hdf5::group_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Property-set {} already exists at {}",
            name, parent.id().pathname()));
    }

    auto group = hdf5::create_group(parent, name);

    create_object_tracker(group);
    link_time_domain(group, domain);
    create_properties(group);

    return PropertySet{std::move(group)};
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    TimeConfiguration const& time_configuration,
    Clock const& clock)
{
    if(hdf5::group_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Property-set {} already exists at {}",
            name, parent.id().pathname()));
    }

    auto group = hdf5::create_group(parent, name);

    create_object_tracker(group);
    create_time_domain(group, time_configuration, clock);
    create_properties(group);

    return PropertySet{std::move(group)};
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    TimeConfiguration const& time_configuration,
    Clock const& clock,
    ObjectTracker& object_tracker)
{
    if(hdf5::group_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Property-set {} already exists at {}",
            name, parent.id().pathname()));
    }

    auto group = hdf5::create_group(parent, name);

    link_object_tracker(group, object_tracker);
    create_time_domain(group, time_configuration, clock);
    create_properties(group);

    return PropertySet{std::move(group)};
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    TimeDomain& domain,
    ObjectTracker& object_tracker)
{
    if(hdf5::group_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Property-set {} already exists at {}",
            name, parent.id().pathname()));
    }

    auto group = hdf5::create_group(parent, name);

    link_object_tracker(group, object_tracker);
    link_time_domain(group, domain);
    create_properties(group);

    return PropertySet{std::move(group)};
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    TimeConfiguration const& time_configuration,
    Clock const& clock,
    SpaceConfiguration const& space_configuration,
    hdf5::Datatype const& space_coordinate_datatype,
    std::size_t const rank)
{
    if(hdf5::group_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Property-set {} already exists at {}",
            name, parent.id().pathname()));
    }

    auto group = hdf5::create_group(parent, name);

    create_object_tracker(group);
    create_time_domain(
        group, time_configuration, clock);
    create_space_domain(
        group, space_configuration, space_coordinate_datatype, rank);
    create_properties(group);

    return PropertySet{std::move(group)};
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    TimeDomain& time_domain,
    SpaceConfiguration const& space_configuration,
    hdf5::Datatype const& space_coordinate_datatype,
    std::size_t rank)
{
    if(hdf5::group_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Property-set {} already exists at {}",
            name, parent.id().pathname()));
    }

    auto group = hdf5::create_group(parent, name);

    create_object_tracker(group);
    link_time_domain(group, time_domain);
    create_space_domain(
        group, space_configuration, space_coordinate_datatype, rank);
    create_properties(group);

    return PropertySet{std::move(group)};
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    TimeDomain& time_domain,
    ObjectTracker& object_tracker,
    SpaceConfiguration const& space_configuration,
    hdf5::Datatype const& space_coordinate_datatype,
    std::size_t rank)
{
    if(hdf5::group_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Property-set {} already exists at {}",
            name, parent.id().pathname()));
    }

    auto group = hdf5::create_group(parent, name);

    link_object_tracker(group, object_tracker);
    link_time_domain(group, time_domain);
    create_space_domain(
        group, space_configuration, space_coordinate_datatype, rank);
    create_properties(group);

    return PropertySet{std::move(group)};
}

}  // namespace data_model
}  // namespace lue
