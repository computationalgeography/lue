#include "lue/properties.h"


namespace lue {

/*!
    @brief      Construct an instance
    @param      location Location where collection is stored
*/
Properties::Properties(
    hdf5::Identifier const& location)

    : Collection(location, "lue_properties")

{
}


Properties::Properties(
    Collection<Property>&& collection)

    : Collection(std::forward<Collection<Property>>(collection))

{
}


Properties create_properties(
    hdf5::Identifier const& location)
{
    return create_collection<Property>(location, "lue_properties");
}

} // namespace lue
