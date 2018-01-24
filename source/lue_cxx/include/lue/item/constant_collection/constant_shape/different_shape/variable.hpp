#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/array.hpp"


namespace lue {
namespace constant_collection {
namespace constant_shape {
namespace different_shape {

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
*/
class Variable:
    public hdf5::Group
{

public:

                     Variable          (hdf5::Group const& parent,
                                        std::string const& name);

                     Variable          (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                     Variable          (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                     Variable          (Variable const&)=delete;

                     Variable          (Variable&&)=default;

                     ~Variable         ()=default;

    Variable&      operator=           (Variable const&)=delete;

    Variable&      operator=           (Variable&&)=default;

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


Variable           create_variable     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

Variable           create_variable     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace constant_collection
}  // namespace lue
