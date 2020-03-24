#include "lue/object/property/different_shape/variable_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {
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


Property& Properties::add(
    std::string const& name,
    hdf5::Datatype const& datatype,
    Rank const rank,
    std::string const& description)
{
    return Collection::add(
        name, create_property(*this, name, datatype, rank, description));
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
}  // namespace data_model
}  // namespace lue
