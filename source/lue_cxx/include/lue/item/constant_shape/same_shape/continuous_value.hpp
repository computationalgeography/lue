#pragma once
#include "lue/array.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {
namespace constant_shape {
namespace same_shape {

/*!
    @brief      This collection manages constant item values for
                collections of values with the same shape

    Use this class if:
    - The collection of items doesn't change through time
    - The shape of the values don't change through time
    - The values of all items have the same shape
    - The values themselves don't change through time

    The underlying HDF5 dataset has one dimension more than the rank of the
    individual values. This first dimension represents the items.

    - a single, same-shaped value per item
    - implies constant collection
    - a single HDF5 dataset for all items

    What         | How
    ------------ | ---
    obtain value | index HDF5 dataset by item idx
    add item     | expand HDF5 dataset
    remove item  | N/A

    `v_i = dataset[i]`, order of items in HDF5 dataset matters
*/
class ContinuousValue:
    public Array
{

public:

                   ContinuousValue     (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   ContinuousValue     (hdf5::Dataset&& dataset,
                                        hdf5::Datatype const& memory_datatype);

                   ContinuousValue     (ContinuousValue const&)=delete;

                   ContinuousValue     (ContinuousValue&&)=default;

                   ~ContinuousValue    ()=default;

    ContinuousValue& operator=         (ContinuousValue const&)=delete;

    ContinuousValue& operator=         (ContinuousValue&&)=default;

    void           reserve             (hsize_t nr_items);

    hsize_t        nr_items            () const;

    hdf5::Shape    value_shape         () const;

    using Array::read;

    void           read                (hsize_t idx,
                                        void* buffer);

    using Array::write;

    void           write               (hsize_t idx,
                                        void const* buffer);

private:

    hdf5::Hyperslab hyperslab          (hsize_t idx) const;

};


ContinuousValue    create_continuous_value(
                                       hdf5::Group& parent,
                                       std::string const& name,
                                       hdf5::Datatype const& memory_datatype);

ContinuousValue    create_continuous_value(
                                       hdf5::Group& parent,
                                       std::string const& name,
                                       hdf5::Datatype const& memory_datatype,
                                       hdf5::Shape const& value_shape);

ContinuousValue    create_continuous_value(
                                       hdf5::Group& parent,
                                       std::string const& name,
                                       hdf5::Datatype const& file_datatype,
                                       hdf5::Datatype const& memory_datatype);

ContinuousValue    create_continuous_value(
                                       hdf5::Group& parent,
                                       std::string const& name,
                                       hdf5::Datatype const& file_datatype,
                                       hdf5::Datatype const& memory_datatype,
                                       hdf5::Shape const& value_shape);

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace lue
