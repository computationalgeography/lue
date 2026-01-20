#include "lue/object/property/same_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model::same_shape {

    Properties::Properties(hdf5::Group const& parent):

        Collection<Property>{parent, same_shape_tag}

    {
    }


    Properties::Properties(Collection<Property>&& collection):

        Collection<Property>{std::move(collection)}

    {
    }


    auto Properties::add(
        std::string const& name, hdf5::Datatype const& datatype, std::string const& description) -> Property&
    {
        return Collection::add(name, create_property(*this, name, datatype, description));
    }


    auto Properties::add(
        std::string const& name,
        hdf5::Datatype const& datatype,
        hdf5::Shape const& shape,
        std::string const& description) -> Property&
    {
        return Collection::add(name, create_property(*this, name, datatype, shape, description));
    }


    auto create_properties(hdf5::Group& parent) -> Properties
    {
        Collection<Property> collection{create_collection<Property>(parent, same_shape_tag)};

        return Properties{std::move(collection)};
    }

}  // namespace lue::data_model::same_shape
