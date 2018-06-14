#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {

/*!
    @brief      Per location in time, an index of first object in active set

    In the ActiveSetIndex dataset, indices can be stored of the first
    object of each active set. The ID of the active objects themselves
    can be stored using the ActiveID class.

    In case of asynchronous occurrence, the size of the active set is alway
    1, and so, this dataset is not necessary.

    In case of a constant collection, only the IDs of a single active
    set is stored and, hence, this dataset is not needed.

    Tracking the indices of active sets is needed in combination with
    the folowing array kinds:
    - same shape x constant shape
    - same shape x variable shape
*/
class ActiveSetIndex:
    public same_shape::constant_shape::Value
{

public:

                   ActiveSetIndex      (hdf5::Group const& parent,
                                        std::string const& name);

                   ActiveSetIndex      (same_shape::constant_shape::Value&&
                                            value);

                   ActiveSetIndex      (ActiveSetIndex const&)=delete;

                   ActiveSetIndex      (ActiveSetIndex&&)=default;

                   ~ActiveSetIndex     ()=default;

    ActiveSetIndex& operator=          (ActiveSetIndex const&)=delete;

    ActiveSetIndex& operator=          (ActiveSetIndex&&)=default;

private:


};


ActiveSetIndex     create_active_set_index(hdf5::Group& parent);

}  // namespace lue
