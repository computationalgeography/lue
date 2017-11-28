#include "lue/properties.hpp"
#include "lue/tag.hpp"


namespace lue {

/*!
    @brief      Construct an instance
    @param      parent Parent group where collection is stored
*/
Properties::Properties(
    hdf5::Group const& parent)

    : Collection(parent, properties_tag)

{
}


Properties::Properties(
    Collection<Property>&& collection)

    : Collection(std::forward<Collection<Property>>(collection))

{
}


Properties create_properties(
    hdf5::Group const& group)
{
    return create_collection<Property>(group, properties_tag);
}

} // namespace lue
