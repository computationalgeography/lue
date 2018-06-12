#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/array.hpp"


namespace lue {
namespace different_shape {

/*!
    @brief      This collection manages item values for
                collections of constant values with a different shape

    Use this class if:
    - The collection of items doesn't change through time
    - The shape of the values don't change through time
    - The shape of the each item's value can be different
    - The values themselves don't change through time

    Each item's value ends up in a seperate HDF5 dataset, named after the
    item's index. All datasets have the same rank, but the size of each
    dimension may be different.

    - a single, different-shaped value per item
    - implies constant collection
    - an HDF5 dataset for each item

    | obtain value | HDF5 dataset named after item id |
    | add item     | create HDF5 dataset              |
    | remove item  | not applicable                   |

    `v_i = dataset_i`
*/
class ContinuousValue:
    public hdf5::Group
{

public:

                   ContinuousValue     (hdf5::Group const& parent,
                                        std::string const& name);

                   ContinuousValue     (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   ContinuousValue     (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   ContinuousValue     (ContinuousValue const&)=delete;

                   ContinuousValue     (ContinuousValue&&)=default;

                   ~ContinuousValue    ()=default;

    ContinuousValue& operator=         (ContinuousValue const&)=delete;

    ContinuousValue& operator=         (ContinuousValue&&)=default;

    hsize_t        nr_items            () const;

    int            rank                () const;

    hdf5::Datatype const&
                   file_datatype       () const;

    hdf5::Datatype const&
                   memory_datatype     () const;

    void           reserve             (hsize_t nr_items,
                                        hsize_t const* value_shapes);

    hdf5::Shape    value_shape         (hsize_t idx);

    void           read                (hsize_t idx,
                                        void* buffer);

    void           write               (hsize_t idx,
                                        void const* buffer);

    Array          operator[]          (hsize_t idx) const;

private:

    void           reserve             (hsize_t idx,
                                        hdf5::Shape const& value_shape);

    //! Number of items for which values are stored
    hsize_t        _nr_items;

    //! Rank of the values
    int            _rank;

    //! In-file datatype
    hdf5::Datatype const _file_datatype;

    //! In-memory datatype
    hdf5::Datatype const _memory_datatype;

};


ContinuousValue       create_continuous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

ContinuousValue       create_continuous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace different_shape
}  // namespace lue
