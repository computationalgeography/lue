#pragma once
#include "lue/object/property_sets.hpp"


namespace lue {

// Try not to use this functionality!
// These are all implementation details. Never depend on the knowledge
// that is encoded in the implementation.

template<
    typename Property>
inline hdf5::Group property_set_group(
    Property& property)
{
    return property.parent().parent().parent();
}


template<
    typename Property>
inline hdf5::Group property_sets_group(
    Property& property)
{
    return property_set_group(property).parent();
}


template<
    typename Property>
inline PropertySet property_set(
    Property& property)
{
    return PropertySet{property_set_group(property)};
}


template<
    typename Property>
inline PropertySets property_sets(
    Property& property)
{
    return PropertySets{
        Collection<PropertySet>{property_sets_group(property)}};
}

}  // namespace lue
