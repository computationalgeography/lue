#pragma once
#include "lue/cxx_api/hdf5/dataset.h"
#include "lue/cxx_api/shape.h"


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
class Array
{

public:

                   Array               (hdf5::Identifier&& location,
                                        hid_t type_id);

                   Array               (Array const& other)=delete;

                   Array               (Array&& other)=default;

                   ~Array              ()=default;

    Array&         operator=           (Array const& other)=delete;

    Array&         operator=           (Array&& other)=default;

    hid_t          type_id             () const;

    hdf5::Identifier const&
                   dataset_id          () const;

    Shape          shape               () const;

    void           read                (hsize_t const start,
                                        hsize_t const count,
                                        hsize_t const stride,
                                        void* buffer) const;

    void           write               (hsize_t const start,
                                        hsize_t const count,
                                        hsize_t const stride,
                                        void const* buffer);

private:

    //! HDF5 dataset.
    hdf5::Dataset  _dataset;

    //! In-memory type-id.
    hid_t          _type_id;

};

} // namespace lue
