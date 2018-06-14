#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {

/*!
    @brief      An index into the value array per object in the active set

    In the ActiveObjectIndex dataset, indices can be stored of objects
    that are active at a given location in time. Multiple sets of indices
    can be stored sequentially. The index of the ID of the first active
    object in a set can be stored using the ActiveSetIndex class.

    Tracking the IDs of the active objects is needed in combination with
    the folowing object array kinds:
    - different shape x constant shape
*/
class ActiveObjectIndex:
    public same_shape::constant_shape::Value
{

public:

                   ActiveObjectIndex   (hdf5::Group const& parent,
                                        std::string const& name);

                   ActiveObjectIndex   (same_shape::constant_shape::Value&&
                                            value);

                   ActiveObjectIndex   (ActiveObjectIndex const&)=delete;

                   ActiveObjectIndex   (ActiveObjectIndex&&)=default;

                   ~ActiveObjectIndex  ()=default;

    ActiveObjectIndex& operator=       (ActiveObjectIndex const&)=delete;

    ActiveObjectIndex& operator=       (ActiveObjectIndex&&)=default;

private:


};


ActiveObjectIndex  create_active_object_index(
                                        hdf5::Group& parent);

}  // namespace lue
