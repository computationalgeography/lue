#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/array.hpp"


namespace lue {
namespace constant_shape {
namespace different_shape {

class Value:
    public hdf5::Group
{

    friend Value   create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

    friend Value   create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

public:

                   Value               (Value const&)=delete;

                   Value               (Value&&)=default;

                   ~Value              ()=default;

    Value&         operator=           (Value const&)=delete;

    Value&         operator=           (Value&&)=default;

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

protected:

                   Value               (hdf5::Group const& parent,
                                        std::string const& name);

                   Value               (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Value               (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

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


Value              create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

Value              create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace lue
