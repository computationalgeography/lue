#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/array.hpp"


namespace lue {
namespace same_shape {
namespace constant_shape {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .

    - different number of same-shaped values per item
    - an HDF5 dataset for each item
    - additional dimension for time domain

    | obtain value | HDF5 dataset named after item id, index by time idx |
    | add item     | create HDF5 dataset                                 |
    | remove item  | don't write values for time idx                     |

    `v_i_t = dataset_i[time_idx]`
*/
class Value:
    public hdf5::Group
{

    friend Value   create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

    friend Value   create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

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
                                        hsize_t const* nr_time_domain_items);

    hsize_t        nr_time_domain_items(hsize_t item_idx) const;

    hsize_t        nr_items            () const;

    hdf5::Shape const&
                   value_shape         ();

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

    hdf5::Shape const _value_shape;

    //! In-file datatype
    hdf5::Datatype const _file_datatype;

    //! In-memory datatype
    hdf5::Datatype const _memory_datatype;

};


Value create_value                     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

Value create_value                     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace constant_shape
}  // namespace same_shape
}  // namespace lue
