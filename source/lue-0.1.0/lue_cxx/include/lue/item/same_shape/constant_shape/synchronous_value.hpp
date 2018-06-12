#pragma once
#include "lue/item/same_shape/constant_shape/value.hpp"


namespace lue {
namespace same_shape {
namespace constant_shape {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .

    - different number of same-shaped values per item
    - an HDF5 dataset for each item
    - additional dimension for time domain

        | obtain value | HDF5 dataset named after item id |
        | add item     | create HDF5 dataset              |
        | remove item  | don't write values for time idx  |

        `v_i_t = dataset_i[time_idx]`

    - works for `constant_collection` and `variable_collection`, but see
        `same_shape/constant_shape/constant_collection/Synchronousvalue`
        for optimization
    - Similar to `same_shape/constant_shape/AsynchronousValue`
    - Inherit from `same_shape/constant_shape/Value`
*/
class SynchronousValue:
    public Value
{

public:

                   SynchronousValue    (hdf5::Group const& parent,
                                        std::string const& name);

                   SynchronousValue    (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   SynchronousValue    (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   SynchronousValue    (SynchronousValue const&)=delete;

                   SynchronousValue    (SynchronousValue&&)=default;

                   ~SynchronousValue   ()=default;

    SynchronousValue& operator=        (SynchronousValue const&)=delete;

    SynchronousValue& operator=        (SynchronousValue&&)=default;

private:

};


SynchronousValue create_synchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

SynchronousValue create_synchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace constant_shape
}  // namespace same_shape
}  // namespace lue
