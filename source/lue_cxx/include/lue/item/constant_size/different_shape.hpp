#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/array.hpp"


namespace lue {
namespace constant_size {

/*!
    @brief      DifferentShape manages item values for constant sized
                collections of values with a different shape

    Each item's value ends up in a seperate HDF5 dataset, named after the
    item's index.
*/
class DifferentShape:
    public hdf5::Group
{

public:

                   DifferentShape      (hdf5::Group const& parent,
                                        std::string const& name);

                   // DifferentShape      (hdf5::Group const& parent,
                   //                      std::string const& name,
                   //                      hdf5::Datatype const& memory_datatype);

                   DifferentShape      (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   DifferentShape      (DifferentShape const&)=delete;

                   DifferentShape      (DifferentShape&&)=default;

                   ~DifferentShape     ()=default;

    DifferentShape& operator=          (DifferentShape const&)=delete;

    DifferentShape& operator=          (DifferentShape&&)=default;

    hsize_t        nr_items            () const;

    int            rank                () const;

    hdf5::Datatype const&
                   file_datatype       () const;

    hdf5::Datatype const&
                   memory_datatype     () const;

    void           reserve             (hsize_t nr_items,
                                        hsize_t const* value_shapes);

    hdf5::Shape    value_shape         (size_t idx);

    void           read                (size_t idx,
                                        void* buffer);

    void           write               (size_t idx,
                                        void const* buffer);

    Array          operator[]          (size_t idx) const;

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


DifferentShape     create_different_shape(
                                        hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

DifferentShape     create_different_shape(
                                        hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace constant_size
}  // namespace lue
