#pragma once
#include "lue/info/property/same_shape/variable_shape/property.hpp"
#include "lue/object/property/properties_traits.hpp"
#include "lue/core/collection.hpp"


namespace lue {
namespace data_model {
namespace same_shape {
namespace variable_shape {

/*!
    @brief      Collection of same shape x variable shape properties
*/
class Properties:
    public Collection<Property>
{

public:

    explicit       Properties          (hdf5::Group& parent);

    explicit       Properties          (Collection<Property>&& collection);

                   Properties          (Properties const&)=delete;

                   Properties          (Properties&&)=default;

                   ~Properties         () override =default;

    Properties&    operator=           (Properties const&)=delete;

    Properties&    operator=           (Properties&&)=default;

    Property&      add                 (std::string const& name,
                                        hdf5::Datatype const& datatype,
                                        Rank rank,
                                        std::string const& description="");

private:

};


Properties         create_properties   (hdf5::Group& parent);

}  // namespace variable_shape
}  // namespace same_shape


template<>
class PropertyTraits<same_shape::variable_shape::Properties>
{

public:

    using Property = same_shape::variable_shape::Properties::Element;

    using Value = ValueT<Property>;

};

}  // namespace data_model
}  // namespace lue
