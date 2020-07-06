#include "lue/object/property/same_shape/variable_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {
namespace same_shape {
namespace variable_shape {

Properties::Properties(
    hdf5::Group const& parent):

    Collection<Property>{parent, same_shape_variable_shape_tag}

{
}


Properties::Properties(
    Collection<Property>&& collection):

    Collection<Property>{std::move(collection)}

{
}


Property& Properties::add(
    std::string const& name,
    hdf5::Datatype const& datatype,
    Rank const rank,
    std::string const& description)
{
    return Collection::add(name, create_property(*this, name, datatype, rank, description));
}


Properties create_properties(
    hdf5::Group& parent)
{
    Collection<Property> collection{create_collection<Property>(parent, same_shape_variable_shape_tag)};

    return Properties{std::move(collection)};
}

}  // namespace variable_shape
}  // namespace same_shape
}  // namespace data_model
}  // namespace lue
