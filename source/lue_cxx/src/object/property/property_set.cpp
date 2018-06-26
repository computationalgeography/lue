#include "lue/object/property/property_set.hpp"
#include "lue/core/tag.hpp"


namespace lue {

PropertySet::PropertySet(
    hdf5::Group const& parent):

    hdf5::Group{parent, property_set_tag},
    _time_domain{*this},
    _space_domain{*this},
    _properties{*this}

{
}


PropertySet::PropertySet(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _time_domain{*this},
    _space_domain{*this},
    _properties{*this}

{
}


TimeDomain const& PropertySet::time_domain() const
{
    return _time_domain;
}


SpaceDomain const& PropertySet::space_domain() const
{
    return _space_domain;
}


Properties const& PropertySet::properties() const
{
    return _properties;
}


PropertySet create_property_set(
    hdf5::Group& parent,
    TimeConfiguration const& time_configuration,
    SpaceConfiguration const& space_configuration)
{
    auto group = hdf5::create_group(parent, property_set_tag);

    create_time_domain(group, time_configuration);
    create_space_domain(group, space_configuration);
    create_properties(group);

    return PropertySet{std::move(group)};
}

}  // namespace lue
