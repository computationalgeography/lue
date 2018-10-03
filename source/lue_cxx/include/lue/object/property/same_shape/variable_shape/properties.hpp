#pragma once
#include "lue/info/identity/active_object_id.hpp"
#include "lue/info/identity/active_set_index.hpp"
#include "lue/info/property/same_shape/variable_shape/property.hpp"
#include "lue/core/collection.hpp"


namespace lue {
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

};


Properties         create_properties   (hdf5::Group& parent,
                                        ActiveObjectID& active_object_id,
                                        ActiveSetIndex& active_set_index);

}  // namespace variable_shape
}  // namespace same_shape
}  // namespace lue
