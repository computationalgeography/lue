#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {

/*!
    @brief      An ID per object in the active set
    @param      .
    @return     .
    @exception  .

    In the ActiveID dataset, IDs can be stored of objects that are active
    at a given location in time. Multiple sets of object IDs can be stored
    sequentially. The index of the ID of the first active object in a set
    can be stored using the ActiveSetID class.

    In case related information (IDs of the active objects, locations
    in space, properties) does not change through time, only a single
    set of IDs need to be stored.

    Tracking the IDs of the active objects is needed in combination with
    the folowing object array kinds:
    - same shape x constant value
    - same shape x constant shape
    - same shape x variable shape
*/
class ActiveID:
    public same_shape::constant_shape::Value
{

public:

                   ActiveID            (hdf5::Group const& parent,
                                        std::string const& name);

                   ActiveID            (same_shape::constant_shape::Value&&
                                            value);

                   ActiveID            (ActiveID const&)=delete;

                   ActiveID            (ActiveID&&)=default;

                   ~ActiveID           ()=default;

    ActiveID&      operator=           (ActiveID const&)=delete;

    ActiveID&      operator=           (ActiveID&&)=default;

private:


};


ActiveID           create_active_id    (hdf5::Group& parent);

}  // namespace lue
