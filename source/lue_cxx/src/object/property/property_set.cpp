#include "lue/object/property/property_set.hpp"
#include "lue/core/tag.hpp"


namespace lue {

PropertySet::PropertySet(
    hdf5::Group& parent,
    std::string const& name):

    hdf5::Group{parent, name},
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


ObjectTracker const& PropertySet::object_tracker() const
{
    return _object_tracker;
}


ObjectTracker& PropertySet::object_tracker()
{
    return _object_tracker;
}


bool PropertySet::has_time_domain() const
{
    return _time_domain ? true : false;
}


bool PropertySet::has_space_domain() const
{
    return _space_domain ? true : false;
}


TimeDomain const& PropertySet::time_domain() const
{
    assert(_time_domain);
    return *_time_domain;
}


SpaceDomain const& PropertySet::space_domain() const
{
    assert(_space_domain);
    return *_space_domain;
}


Properties const& PropertySet::properties() const
{
    return _properties;
}


same_shape::Property& PropertySet::add_property(
    std::string const& name,
    hdf5::Datatype const& datatype)
{
    return _properties.add(name, datatype);
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name)
{
    auto group = hdf5::create_group(parent, name);

    auto object_tracker = create_object_tracker(group);
    create_properties(group, object_tracker);

    return PropertySet{std::move(group)};
}


PropertySet create_property_set(
    hdf5::Group& parent,
    std::string const& name,
    TimeConfiguration const& time_configuration,
    SpaceConfiguration const& space_configuration)
{
    auto group = hdf5::create_group(parent, name);

    auto object_tracker = create_object_tracker(group);
    create_time_domain(group, time_configuration);
    create_space_domain(group, space_configuration);
    create_properties(group, object_tracker);

    return PropertySet{std::move(group)};
}

}  // namespace lue
