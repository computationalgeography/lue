#include "lue/object/property/same_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {
namespace same_shape {

Properties::Properties(
    hdf5::Group& parent):

    Collection<Property>{parent, same_shape_tag}

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
    std::string const& description)
{
    return Collection::add(
        name, create_property(*this, name, datatype, description));
}


Property& Properties::add(
    std::string const& name,
    hdf5::Datatype const& datatype,
    hdf5::Shape const& shape,
    std::string const& description)
{
    return Collection::add(
        name, create_property(*this, name, datatype, shape, description));
}


Properties create_properties(
    hdf5::Group& parent)
{
    auto collection = create_collection<Property>(parent, same_shape_tag);

    return Properties{std::move(collection)};
}

}  // namespace same_shape
}  // namespace data_model
}  // namespace lue
