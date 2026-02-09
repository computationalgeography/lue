#include "lue/object/property_sets.hpp"


namespace lue::data_model {

    PropertySets::PropertySets(hdf5::Group const& parent, std::string const& name):

        Collection<PropertySet>{parent, name}

    {
    }


    PropertySets::PropertySets(Collection<PropertySet>&& collection):

        Collection<PropertySet>{std::move(collection)}

    {
    }


    auto PropertySets::add(std::string const& name) -> PropertySet&
    {
        return Collection::add(name, create_property_set(*this, name));
    }


    auto PropertySets::add(
        std::string const& name,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t const rank) -> PropertySet&
    {
        return Collection::add(
            name, create_property_set(*this, name, space_configuration, space_coordinate_datatype, rank));
    }


    auto PropertySets::add(
        std::string const& name, TimeConfiguration const& time_configuration, Clock const& clock)
        -> PropertySet&
    {
        return Collection::add(name, create_property_set(*this, name, time_configuration, clock));
    }


    auto PropertySets::add(std::string const& name, TimeDomain& domain) -> PropertySet&
    {
        return Collection::add(name, create_property_set(*this, name, domain));
    }


    auto PropertySets::add(
        std::string const& name,
        TimeConfiguration const& time_configuration,
        Clock const& clock,
        ObjectTracker& object_tracker) -> PropertySet&
    {
        return Collection::add(
            name, create_property_set(*this, name, time_configuration, clock, object_tracker));
    }


    auto PropertySets::add(std::string const& name, TimeDomain& domain, ObjectTracker& object_tracker)
        -> PropertySet&
    {
        return Collection::add(name, create_property_set(*this, name, domain, object_tracker));
    }


    auto PropertySets::add(
        std::string const& name,
        TimeConfiguration const& time_configuration,
        Clock const& clock,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t const rank) -> PropertySet&
    {
        return Collection::add(
            name,
            create_property_set(
                *this,
                name,
                time_configuration,
                clock,
                space_configuration,
                space_coordinate_datatype,
                rank));
    }


    auto PropertySets::add(
        std::string const& name,
        TimeDomain& time_domain,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t rank) -> PropertySet&
    {
        return Collection::add(
            name,
            create_property_set(
                *this, name, time_domain, space_configuration, space_coordinate_datatype, rank));
    }


    auto PropertySets::add(
        std::string const& name,
        TimeDomain& time_domain,
        ObjectTracker& object_tracker,
        SpaceConfiguration const& space_configuration,
        hdf5::Datatype const& space_coordinate_datatype,
        std::size_t rank) -> PropertySet&
    {
        return Collection::add(
            name,
            create_property_set(
                *this,
                name,
                time_domain,
                object_tracker,
                space_configuration,
                space_coordinate_datatype,
                rank));
    }


    auto create_property_sets(hdf5::Group& parent, std::string const& name) -> PropertySets
    {
        if (collection_exists(parent, name))
        {
            throw std::runtime_error(
                std::format("Property-set collection {} already exists at {}", name, parent.id().pathname()));
        }


        return PropertySets{create_collection<PropertySet>(parent, name)};
    }

}  // namespace lue::data_model
