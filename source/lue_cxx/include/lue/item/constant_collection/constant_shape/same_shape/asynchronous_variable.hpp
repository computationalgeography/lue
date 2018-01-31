#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/array.hpp"


namespace lue {
namespace constant_collection {
namespace constant_shape {
namespace same_shape {

class AsynchronousVariable:
    public hdf5::Group
{

public:

                   AsynchronousVariable(hdf5::Group const& parent,
                                        std::string const& name);

                   AsynchronousVariable(hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   AsynchronousVariable(hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   AsynchronousVariable(AsynchronousVariable const&)=delete;

                   AsynchronousVariable(AsynchronousVariable&&)=default;

                   ~AsynchronousVariable()=default;

    AsynchronousVariable&
                   operator=           (AsynchronousVariable const&)=delete;

    AsynchronousVariable&
                   operator=           (AsynchronousVariable&&)=default;

    hdf5::Datatype const&
                   file_datatype       () const;

    hdf5::Datatype const&
                   memory_datatype     () const;

    void           reserve             (hsize_t nr_items,
                                        hsize_t const* nr_time_domain_items,
                                        hsize_t const* value_shapes);

    hsize_t        nr_time_domain_items(hsize_t item_idx) const;

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

    //! Number of items for which values are stored
    hsize_t        _nr_items;

    //! Rank of the values
    int const      _rank;

    //! In-file datatype
    hdf5::Datatype const _file_datatype;

    //! In-memory datatype
    hdf5::Datatype const _memory_datatype;

};


AsynchronousVariable create_asynchronous_variable(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

AsynchronousVariable create_asynchronous_variable(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace constant_collection
}  // namespace lue
