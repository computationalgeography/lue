#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {
namespace info {

/*!
    @brief      Per location in time, an index of first object in active set

    In the ActiveSetIndex dataset, indices are stored of the first
    object of each active set. The ID of the active objects themselves
    can be stored using the ActiveID class.

    Tracking the indices of active sets is needed in combination with
    the folowing array kinds:
    - same_shape::constant_shape::Value
    - same_shape::variable_shape::Value
    - different_shape::constant_shape::Value
*/
class ActiveSetIndex:
    public same_shape::constant_shape::Value
{

public:

    friend ActiveSetIndex create_active_set_index(hdf5::Group& parent);

                   ActiveSetIndex      (hdf5::Group& parent);

                   ActiveSetIndex      (ActiveSetIndex const&)=delete;

                   ActiveSetIndex      (ActiveSetIndex&&)=default;

                   ~ActiveSetIndex     ()=default;

    ActiveSetIndex& operator=          (ActiveSetIndex const&)=delete;

    ActiveSetIndex& operator=          (ActiveSetIndex&&)=default;

private:

                   ActiveSetIndex      (same_shape::constant_shape::Value&&
                                            value);


};


ActiveSetIndex     create_active_set_index(hdf5::Group& parent);

}  // namespace info
}  // namespace lue
