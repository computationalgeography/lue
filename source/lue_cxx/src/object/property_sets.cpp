#include "lue/object/property_sets.hpp"


namespace lue {

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
    SpaceConfiguration const& space_configuration)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name, space_configuration));
}


PropertySet& PropertySets::add(
    std::string const& name,
    TimeConfiguration const& time_configuration,
    SpaceConfiguration const& space_configuration)
{
    return Collection::add(
        name,
        create_property_set(
            *this, name, time_configuration, space_configuration));
}


PropertySets create_property_sets(
    hdf5::Group& parent,
    std::string const& name)
{
    auto collection = create_collection<PropertySet>(
        parent, name);

    return PropertySets{std::move(collection)};
}

} // namespace lue
