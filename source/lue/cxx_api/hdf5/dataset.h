#pragma once
#include "lue/cxx_api/hdf5/dataspace.h"
#include "lue/cxx_api/hdf5/identifier.h"


namespace lue {
namespace hdf5 {

/*!
    @ingroup    lue_cxx_api_hdf5_group
*/
class Dataset
{

public:

                   Dataset             (Identifier&& id);

                   Dataset             (Dataset const& other)=delete;

                   Dataset             (Dataset&& other)=default;

                   ~Dataset            ()=default;

    Dataset&       operator=           (Dataset const& other)=delete;

    Dataset&       operator=           (Dataset&& other)=default;

    Identifier const& id               () const;

    Dataspace      dataspace           () const;

    void           read                (hid_t const type_id,
                                        hsize_t const start,
                                        hsize_t const count,
                                        hsize_t const stride,
                                        void* buffer) const;

    void           write               (hid_t const type_id,
                                        hsize_t const start,
                                        hsize_t const count,
                                        hsize_t const stride,
                                        void const* buffer) const;

private:

    Identifier     _id;

};

} // namespace hdf5
} // namespace lue
