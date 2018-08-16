#pragma once
#include "lue/info/property/different_shape/variable_shape/property.hpp"
#include "lue/core/collection.hpp"


namespace lue {
namespace different_shape {
namespace variable_shape {

/*!
    @brief      Collection of different shape x variable shape properties
*/
class Properties:
    public Collection<Property>
{

public:

    explicit       Properties          (hdf5::Group& parent);

                   Properties          (Collection<Property>&& collection);

                   Properties          (Properties const&)=delete;

                   Properties          (Properties&&)=default;

                   ~Properties         ()=default;

    Properties&    operator=           (Properties const&)=delete;

    Properties&    operator=           (Properties&&)=default;

    Property&      add                 (std::string const& name,
                                        hdf5::Datatype const& datatype,
                                        Rank rank);

private:

};


Properties         create_properties   (hdf5::Group& parent);

}  // namespace variable_shape
}  // namespace different_shape
}  // namespace lue
