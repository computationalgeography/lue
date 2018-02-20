#pragma once
#include "lue/array.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {
namespace same_shape {
namespace constant_shape {
namespace constant_collection {

/*!
    @brief      This collection manages variable item values for constant
                sized collections of values with the same shape

    Use this class if:
    - The collection of items doesn't change through time
    - The shape of the values don't change through time
    - The values of all items have the same shape
    - The values themselves change through time

    The underlying HDF5 dataset has two dimensions more than the rank
    of the individual values. The first dimension represents the time,
    the second the items.

    - same number of same-shaped values per item
    - a single HDF5 dataset for all items
    - no additional dimension for time domain

    | obtain value | index HDF5 dataset by item idx and time idx |
    | add item     | not possible                                |
    | remove item  | not applicable                              |

    `v_i_t = dataset[time_idx][item_idx]`, order matters

    - Optimization of `same_shape/constant_shape/SynchronousValue` in case
        the collection of items doesn't change through time
*/
class SynchronousValue:
    public Array
{

public:

                   SynchronousValue    (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   SynchronousValue    (hdf5::Dataset&& dataset,
                                        hdf5::Datatype const& memory_datatype);

                   SynchronousValue    (SynchronousValue const&)=delete;

                   SynchronousValue    (SynchronousValue&&)=default;

                   ~SynchronousValue   ()=default;

    SynchronousValue&
                   operator=           (SynchronousValue const&)=delete;

    SynchronousValue&
                   operator=           (SynchronousValue&&)=default;

    void           reserve             (hsize_t nr_items,
                                        hsize_t nr_time_domain_items);

    hsize_t        nr_time_domain_items() const;

    hsize_t        nr_items            () const;

    hdf5::Shape    value_shape         () const;

    using Array::read;

    void           read                (hsize_t item_idx,
                                        hsize_t time_idx,
                                        void* buffer);

    using Array::write;

    void           write               (hsize_t item_idx,
                                        hsize_t time_idx,
                                        void const* buffer);

private:

    hdf5::Hyperslab hyperslab          (hsize_t item_idx) const;

    hdf5::Hyperslab hyperslab          (hsize_t item_idx,
                                        hsize_t time_idx) const;

};


SynchronousValue create_synchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

SynchronousValue create_synchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

SynchronousValue create_synchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

SynchronousValue create_synchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace constant_collection
}  // namespace constant_shape
}  // namespace same_shape
}  // namespace lue
