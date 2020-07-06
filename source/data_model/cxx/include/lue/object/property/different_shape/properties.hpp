#pragma once
#include "lue/info/property/different_shape/property.hpp"
#include "lue/object/property/properties_traits.hpp"
#include "lue/core/collection.hpp"


namespace lue {
namespace data_model {
namespace different_shape {

/*!
    @brief      Collection of different shape x constant value properties
*/
class Properties:
    public Collection<Property>
{

public:

    explicit       Properties          (hdf5::Group const& parent);

    explicit       Properties          (Collection<Property>&& collection);

                   Properties          (Properties const&)=default;

                   Properties          (Properties&&)=default;

                   ~Properties         () override =default;

    Properties&    operator=           (Properties const&)=default;

    Properties&    operator=           (Properties&&)=default;

    Property&      add                 (std::string const& name,
                                        hdf5::Datatype const& datatype,
                                        Rank rank,
                                        std::string const& description="");

private:

};


Properties         create_properties   (hdf5::Group& parent);

}  // namespace different_shape


template<>
class PropertyTraits<different_shape::Properties>
{

public:

    using Property = different_shape::Properties::Element;

    using Value = ValueT<Property>;

};

}  // namespace data_model
}  // namespace lue
