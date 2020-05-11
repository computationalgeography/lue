#include "lue/object/property_sets.hpp"


namespace lue {
namespace data_model {

PropertySets::PropertySets(
    hdf5::Group& parent,
    std::string const& name):

    Collection<PropertySet>{parent, name}

{
}


PropertySets::PropertySets(
    Collection<PropertySet>&& collection):

    Collection<PropertySet>{
        std::forward<Collection<PropertySet>>(collection)}

{
}


PropertySet& PropertySets::add(
    std::string const& name)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name));
}


PropertySet& PropertySets::add(
    std::string const& name,
    SpaceConfiguration const& space_configuration,
    hdf5::Datatype const& space_coordinate_datatype,
    std::size_t const rank)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name,
            space_configuration, space_coordinate_datatype, rank));
}


PropertySet& PropertySets::add(
    std::string const& name,
    TimeConfiguration const& time_configuration,
    Clock const& clock)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name,
            time_configuration, clock));
}


PropertySet& PropertySets::add(
    std::string const& name,
    TimeDomain& domain)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name,
            domain));
}


PropertySet& PropertySets::add(
    std::string const& name,
    TimeConfiguration const& time_configuration,
    Clock const& clock,
    ObjectTracker& object_tracker)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name,
            time_configuration, clock, object_tracker));
}


PropertySet& PropertySets::add(
    std::string const& name,
    TimeDomain& domain,
    ObjectTracker& object_tracker)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name,
            domain, object_tracker));
}


PropertySet& PropertySets::add(
    std::string const& name,
    TimeConfiguration const& time_configuration,
    Clock const& clock,
    SpaceConfiguration const& space_configuration,
    hdf5::Datatype const& space_coordinate_datatype,
    std::size_t const rank)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name,
            time_configuration, clock,
            space_configuration, space_coordinate_datatype, rank));
}


PropertySet& PropertySets::add(
    std::string const& name,
    TimeDomain& time_domain,
    SpaceConfiguration const& space_configuration,
    hdf5::Datatype const& space_coordinate_datatype,
    std::size_t rank)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name,
            time_domain,
            space_configuration, space_coordinate_datatype, rank));
}


PropertySet& PropertySets::add(
    std::string const& name,
    TimeDomain& time_domain,
    ObjectTracker& object_tracker,
    SpaceConfiguration const& space_configuration,
    hdf5::Datatype const& space_coordinate_datatype,
    std::size_t rank)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name,
            time_domain, object_tracker,
            space_configuration, space_coordinate_datatype, rank));
}


PropertySets create_property_sets(
    hdf5::Group& parent,
    std::string const& name)
{
    if(collection_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Property-set collection {} already exists at {}",
            name, parent.id().pathname()));
    }


    return PropertySets{create_collection<PropertySet>(parent, name)};
}

} // namespace data_model
} // namespace lue
