#pragma once
#include "lue/array.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {
namespace constant_collection {
namespace constant_shape {
namespace same_shape {

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
*/
class SynchronousVariable:
    public Array
{

public:

                   SynchronousVariable (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   SynchronousVariable (hdf5::Dataset&& dataset,
                                        hdf5::Datatype const& memory_datatype);

                   SynchronousVariable (SynchronousVariable const&)=delete;

                   SynchronousVariable (SynchronousVariable&&)=default;

                   ~SynchronousVariable()=default;

    SynchronousVariable&
                   operator=           (SynchronousVariable const&)=delete;

    SynchronousVariable&
                   operator=           (SynchronousVariable&&)=default;

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


SynchronousVariable create_synchronous_variable(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

SynchronousVariable create_synchronous_variable(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

SynchronousVariable create_synchronous_variable(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

SynchronousVariable create_synchronous_variable(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace constant_collection
}  // namespace lue
