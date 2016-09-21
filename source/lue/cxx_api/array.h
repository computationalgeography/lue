#pragma once
#include "lue/cxx_api/hdf5/dataset.h"
#include "lue/cxx_api/shape.h"


namespace lue {

bool               array_exists        (hdf5::Identifier const& location,
                                        std::string const& name);


/*!
    @ingroup    lue_cxx_api_group
*/
class Array:
    public hdf5::Dataset
{

public:

                   // Array               (hdf5::Identifier&& location,
                   //                      hid_t const type_id);

                   Array               (hdf5::Dataset&& dataset,
                                        hid_t const type_id);

                   Array               (Array const& other)=delete;

                   Array               (Array&& other)=default;

                   ~Array              ()=default;

    Array&         operator=           (Array const& other)=delete;

    Array&         operator=           (Array&& other)=default;

    Shape          shape               () const;

    void           read                (std::vector<hsize_t> const& start,
                                        std::vector<hsize_t> const& count,
                                        std::vector<hsize_t> const& stride,
                                        void* buffer) const;

    void           write               (std::vector<hsize_t> const& start,
                                        std::vector<hsize_t> const& count,
                                        std::vector<hsize_t> const& stride,
                                        void const* buffer);

private:

    //! In-memory type-id.
    hid_t          _type_id;

};


// Array              create_array        (hdf5::Identifier const& location,
//                                         std::string const& name,
//                                         hid_t const datatype,
//                                         hid_t const dataspace);

Array              open_array          (hdf5::Identifier const& location,
                                        std::string const& name);

Array              open_array          (hdf5::Identifier const& location,
                                        std::string const& name,
                                        hid_t const type_id);

} // namespace lue
