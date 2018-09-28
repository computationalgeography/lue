#pragma once
#include "lue/info/identity/active_object_id.hpp"
#include "lue/info/identity/active_set_index.hpp"
#include "lue/info/property/same_shape/constant_shape/property.hpp"
#include "lue/core/collection.hpp"


namespace lue {
namespace same_shape {
namespace constant_shape {

/*!
    @brief      Collection of same shape x constant shape properties
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
                                        hdf5::Datatype const& datatype);

    Property&      add                 (std::string const& name,
                                        hdf5::Datatype const& datatype,
                                        hdf5::Shape const& shape);

private:

    ActiveObjectID _active_object_id;

    info::ActiveSetIndex _active_set_index;

};


Properties         create_properties   (hdf5::Group& parent,
                                        ActiveObjectID& active_object_id,
                                        info::ActiveSetIndex& active_set_index);

}  // namespace constant_shape
}  // namespace same_shape
}  // namespace lue
