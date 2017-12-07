#pragma once
#include "lue/array.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {
namespace constant_size {
namespace variable {
namespace constant_shape {
namespace same_shape {

/*!
    @brief      This collection manages variable item values for constant
                sized collections of values with the same shape

    The underlying HDF5 dataset has two dimensions more than the rank
    of the individual values. The first dimension represents the time,
    the second the items.
*/
class Collection:
    public Array
{

public:

                   Collection          (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Collection          (hdf5::Dataset&& dataset,
                                        hdf5::Datatype const& memory_datatype);

                   Collection          (Collection const&)=delete;

                   Collection          (Collection&&)=default;

                   ~Collection         ()=default;

    Collection&    operator=           (Collection const&)=delete;

    Collection&    operator=           (Collection&&)=default;

    void           reserve             (hsize_t nr_time_domain_items,
                                        hsize_t nr_items);

    hsize_t        nr_time_domain_items() const;

    hsize_t        nr_items            () const;

    hdf5::Shape    value_shape         () const;

    using Array::read;

    void           read                (hsize_t time_idx,
                                        hsize_t item_idx,
                                        void* buffer);

    using Array::write;

    void           write               (hsize_t time_idx,
                                        hsize_t item_idx,
                                        void const* buffer);

private:

    hdf5::Hyperslab hyperslab          (hsize_t item_idx) const;

    hdf5::Hyperslab hyperslab          (hsize_t time_idx,
                                        hsize_t item_idx) const;

};


Collection         create_collection   (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

Collection         create_collection   (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

Collection         create_collection   (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

Collection         create_collection   (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace variable
}  // namespace constant_size
}  // namespace lue
