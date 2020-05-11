#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {
namespace data_model {

/*!
    @brief      An ID per object in the active set
    @todo       Use private inheritance to hide the base class' API?

    In the ActiveObjectID dataset, IDs are stored of objects that are active
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
class ActiveObjectID:
    public same_shape::constant_shape::Value
{

public:

    friend ActiveObjectID
                   create_active_object_id(
                                        hdf5::Group& parent);

    explicit       ActiveObjectID      (hdf5::Group& parent);

                   ActiveObjectID      (ActiveObjectID const&)=delete;

                   ActiveObjectID      (ActiveObjectID&&)=default;

                   ~ActiveObjectID     () override =default;

    ActiveObjectID& operator=          (ActiveObjectID const&)=delete;

    ActiveObjectID& operator=          (ActiveObjectID&&)=default;

    Count          nr_ids              () const;

private:

    explicit       ActiveObjectID      (same_shape::constant_shape::Value&&
                                            value);

};


ActiveObjectID     create_active_object_id(
                                        hdf5::Group& parent);

}  // namespace data_model
}  // namespace lue
