#pragma once
#include "lue/array.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {
namespace constant_collection {
namespace constant_shape {
namespace same_shape {

/*!
    @brief      This collection manages constant item values for
                constant sized collections of values with the same shape

    Use this class if:
    - The collection of items doesn't change through time
    - The shape of the values don't change through time
    - The values of all items have the same shape
    - The values themselves don't change through time

    The underlying HDF5 dataset has one dimension more than the rank of the
    individual values. This first dimension represents the items.
*/
class Constant:
    public Array
{

public:

                   Constant            (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Constant            (hdf5::Dataset&& dataset,
                                        hdf5::Datatype const& memory_datatype);

                   Constant            (Constant const&)=delete;

                   Constant            (Constant&&)=default;

                   ~Constant           ()=default;

    Constant&      operator=           (Constant const&)=delete;

    Constant&      operator=           (Constant&&)=default;

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


Constant           create_constant    (hdf5::Group& parent,
                                       std::string const& name,
                                       hdf5::Datatype const& memory_datatype);

Constant           create_constant    (hdf5::Group& parent,
                                       std::string const& name,
                                       hdf5::Datatype const& memory_datatype,
                                       hdf5::Shape const& value_shape);

Constant           create_constant    (hdf5::Group& parent,
                                       std::string const& name,
                                       hdf5::Datatype const& file_datatype,
                                       hdf5::Datatype const& memory_datatype);

Constant           create_constant    (hdf5::Group& parent,
                                       std::string const& name,
                                       hdf5::Datatype const& file_datatype,
                                       hdf5::Datatype const& memory_datatype,
                                       hdf5::Shape const& value_shape);

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace constant_collection
}  // namespace lue
