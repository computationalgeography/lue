#include "lue/object/property/different_shape/variable_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace different_shape {
namespace variable_shape {

Properties::Properties(
    hdf5::Group& parent):

    Collection<Property>{parent, different_shape_variable_shape_tag}

{
}


Properties::Properties(
    Collection<Property>&& collection):

    Collection<Property>{std::forward<Collection<Property>>(collection)}

{
}


Properties create_properties(
    hdf5::Group& parent)
{
    auto collection = create_collection<Property>(
        parent, different_shape_variable_shape_tag);

    return Properties{std::move(collection)};
}

}  // namespace variable_shape
}  // namespace different_shape
}  // namespace lue
