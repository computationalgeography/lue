#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/array.hpp"


namespace lue {
namespace constant_size {
namespace variable {
namespace constant_shape {
namespace different_shape {

/*!
    @brief      This collection manages variable item values for
                constant sized collections of values with a different shape

    Each item's value ends up in a seperate HDF5 dataset, named after the
    item's index. All datasets have the same rank, but the size of each
    dimension may be different. Each dataset has one dimension more than the
    rank of the individual values. This dimension represents time.
*/
class Collection:
    public hdf5::Group
{

public:

                   Collection          (hdf5::Group const& parent,
                                        std::string const& name);

                   Collection          (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Collection          (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   Collection          (Collection const&)=delete;

                   Collection          (Collection&&)=default;

                   ~Collection         ()=default;

    Collection&    operator=           (Collection const&)=delete;

    Collection&    operator=           (Collection&&)=default;

    hdf5::Datatype const&
                   file_datatype       () const;

    hdf5::Datatype const&
                   memory_datatype     () const;

    void           reserve             (hsize_t nr_time_domain_items,
                                        hsize_t nr_items,
                                        hsize_t const* value_shapes);

    hsize_t        nr_time_domain_items() const;

    hsize_t        nr_items            () const;

    int            rank                () const;

    hdf5::Shape    value_shape         (hsize_t item_idx);

    Array          operator[]          (hsize_t item_idx) const;

    void           read                (hsize_t item_idx,
                                        void* buffer);

    void           read                (hsize_t time_idx,
                                        hsize_t item_idx,
                                        void* buffer);

    void           write               (hsize_t item_idx,
                                        void const* buffer);

    void           write               (hsize_t time_idx,
                                        hsize_t item_idx,
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


Collection     create_collection       (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

Collection     create_collection       (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace variable
}  // namespace constant_size
}  // namespace lue
