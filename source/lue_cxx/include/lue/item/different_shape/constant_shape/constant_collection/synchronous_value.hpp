#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/array.hpp"


namespace lue {
namespace different_shape {
namespace constant_shape {
namespace constant_collection {

/*!
    @brief      This collection manages variable item values for
                constant sized collections of values with a different shape

    Use this class if:
    - The collection of items doesn't change through time
    - The shape of the values don't change through time
    - The shape of the each item's value can be different
    - The values themselves change through time

    Each item's value ends up in a seperate HDF5 dataset, named after the
    item's index. All datasets have the same rank, but the size of each
    dimension may be different. Each dataset has one dimension more than the
    rank of the individual values. This dimension represents time.

    - same number of different-shaped values per item
    - an HDF5 dataset for each item
    - no additional dimension for time domain

        | obtain value | HDF5 dataset named after item id, index by time idx |
        | add item     | not possible                                        |
        | remove item  | not possible                                        |

        `v_i_t = dataset_i[time_idx]`

    - Optimization of `constant_shape/different_shape/SynchronousValue` in case
        the collection of items doesn't change through time
*/
class SynchronousValue:
    public hdf5::Group
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

    hdf5::Datatype const&
                   file_datatype       () const;

    hdf5::Datatype const&
                   memory_datatype     () const;

    void           reserve             (hsize_t nr_items,
                                        hsize_t nr_time_domain_items,
                                        hsize_t const* value_shapes);

    hsize_t        nr_time_domain_items() const;

    hsize_t        nr_items            () const;

    int            rank                () const;

    hdf5::Shape    value_shape         (hsize_t item_idx);

    Array          operator[]          (hsize_t item_idx) const;

    void           read                (hsize_t item_idx,
                                        void* buffer);

    void           read                (hsize_t item_idx,
                                        hsize_t time_idx,
                                        void* buffer);

    void           write               (hsize_t item_idx,
                                        void const* buffer);

    void           write               (hsize_t item_idx,
                                        hsize_t time_idx,
                                        void const* buffer);

private:

    void           reserve_value       (hsize_t item_idx,
                                        hsize_t nr_time_domain_items,
                                        hdf5::Shape const& value_shape);

    hdf5::Hyperslab hyperslab          (hdf5::Shape const& dataset_shape,
                                        hsize_t time_idx) const;

    //! Number of time domain items for which values are stored
    hsize_t        _nr_time_domain_items;

    //! Number of items for which values are stored
    hsize_t        _nr_items;

    //! Rank of the values
    int            _rank;

    //! In-file datatype
    hdf5::Datatype const _file_datatype;

    //! In-memory datatype
    hdf5::Datatype const _memory_datatype;

};


SynchronousValue create_synchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

SynchronousValue create_synchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace constant_collection
}  // namespace constant_shape
}  // namespace different_shape
}  // namespace lue
