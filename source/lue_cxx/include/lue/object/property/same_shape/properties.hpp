#pragma once
#include "lue/info/identity/id.hpp"
#include "lue/info/property/same_shape/property.hpp"
#include "lue/core/collection.hpp"


namespace lue {
namespace same_shape {

/*!
    @brief      Collection of same shape x constant value properties
*/
class Properties:
    public Collection<Property>
{

public:

    explicit       Properties          (hdf5::Group const& parent);

                   Properties          (Collection<Property>&& collection);

                   Properties          (Properties const&)=delete;

                   Properties          (Properties&&)=default;

                   ~Properties         ()=default;

    Properties&    operator=           (Properties const&)=delete;

    Properties&    operator=           (Properties&&)=default;

private:

    info::ID       _id;

};


Properties         create_properties   (hdf5::Group& parent,
                                        info::ID& id);

}  // namespace same_shape
}  // namespace lue
