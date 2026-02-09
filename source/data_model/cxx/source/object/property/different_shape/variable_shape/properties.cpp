#include "lue/object/property/different_shape/variable_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model::different_shape::variable_shape {

    Properties::Properties(hdf5::Group const& parent):

        Collection<Property>{parent, different_shape_variable_shape_tag}

    {
    }


    Properties::Properties(Collection<Property>&& collection):

        Collection<Property>{std::move(collection)}

    {
    }


    auto Properties::add(
        std::string const& name,
        hdf5::Datatype const& datatype,
        Rank const rank,
        std::string const& description) -> Property&
    {
        return Collection::add(name, create_property(*this, name, datatype, rank, description));
    }


    auto create_properties(hdf5::Group& parent) -> Properties
    {
        Collection<Property> collection{
            create_collection<Property>(parent, different_shape_variable_shape_tag)};

        return Properties{std::move(collection)};
    }

}  // namespace lue::data_model::different_shape::variable_shape
