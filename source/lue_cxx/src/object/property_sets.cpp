#include "lue/object/property_sets.hpp"
#include "lue/core/tag.hpp"


namespace lue {

PropertySets::PropertySets(
    hdf5::Group const& parent):

    Collection<PropertySet>{parent, property_sets_tag}

{
}


PropertySets::PropertySets(
    Collection<PropertySet>&& collection):

    Collection<PropertySet>{
        std::forward<Collection<PropertySet>>(collection)}

{
}


PropertySets create_property_sets(
    hdf5::Group& parent)
{
    auto collection = create_collection<PropertySet>(
        parent, property_sets_tag);

    return PropertySets{std::move(collection)};
}

} // namespace lue
