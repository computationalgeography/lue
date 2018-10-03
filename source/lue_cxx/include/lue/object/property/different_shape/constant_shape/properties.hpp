#pragma once
#include "lue/info/identity/active_object_id.hpp"
#include "lue/info/identity/active_object_index.hpp"
#include "lue/info/identity/active_set_index.hpp"
#include "lue/info/property/different_shape/constant_shape/property.hpp"
#include "lue/core/collection.hpp"


namespace lue {
namespace different_shape {
namespace constant_shape {

/*!
    @brief      Collection of different shape x constant shape properties
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

    ActiveObjectID _active_object_id;

    ActiveSetIndex _active_set_index;

    ActiveObjectIndex _active_object_index;

};


Properties         create_properties   (hdf5::Group& parent,
                                        ActiveObjectID& active_object_id,
                                        ActiveSetIndex& active_set_index,
                                        ActiveObjectIndex& active_object_index);

}  // namespace constant_shape
}  // namespace different_shape
}  // namespace lue
