#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {
namespace info {

/*!
    @brief      An ID per object in the active set
    @todo       Use private inheritance to hide the base class' API?

    In the ActiveID dataset, IDs are stored of objects that are active
    at a given location in time. Multiple sets of object IDs can be stored
    sequentially. The index of the ID of the first active object in a set
    can be stored using the ActiveSetIndex class.

    In case related information (IDs of the active objects, locations
    in space, properties) does not change through time, only a single
    set of IDs need to be stored. In that case, ActiveSetIndex is
    not needed.

    Tracking the IDs of the active objects is needed in combination with
    the folowing object array kinds:
    - same_shape::constant_shape::Value
    - same_shape::variable_shape::Value
    - different_shape::constant_shape::Value
*/
class ActiveID:
    public same_shape::constant_shape::Value
{

public:

    friend ActiveID create_active_id(hdf5::Group& parent);

                   ActiveID            (hdf5::Group& parent);

                   ActiveID            (ActiveID const&)=delete;

                   ActiveID            (ActiveID&&)=default;

                   ~ActiveID           ()=default;

    ActiveID&      operator=           (ActiveID const&)=delete;

    ActiveID&      operator=           (ActiveID&&)=default;

    Count          nr_ids              () const;

private:

                   ActiveID            (same_shape::constant_shape::Value&&
                                            value);


};


ActiveID           create_active_id    (hdf5::Group& parent);

}  // namespace info
}  // namespace lue
