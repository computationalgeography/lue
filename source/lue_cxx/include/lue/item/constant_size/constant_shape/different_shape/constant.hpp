#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/array.hpp"


namespace lue {
namespace constant_size {
namespace constant_shape {
namespace different_shape {

/*!
    @brief      This collection manages item values for constant sized
                collections of constant values with a different shape

    Each item's value ends up in a seperate HDF5 dataset, named after the
    item's index. All datasets have the same rank, but the size of each
    dimension may be different.
*/
class Constant:
    public hdf5::Group
{

public:

                   Constant            (hdf5::Group const& parent,
                                        std::string const& name);

                   Constant            (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Constant            (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   Constant            (Constant const&)=delete;

                   Constant            (Constant&&)=default;

                   ~Constant           ()=default;

    Constant&      operator=           (Constant const&)=delete;

    Constant&      operator=           (Constant&&)=default;

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


Constant       create_constant         (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

Constant       create_constant         (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace constant_size
}  // namespace lue
