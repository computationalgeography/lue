#pragma once
#include "lue/cxx_api/hdf5/dataspace.h"
#include "lue/cxx_api/hdf5/identifier.h"


namespace lue {
namespace hdf5 {

bool               dataset_exists      (hdf5::Identifier const& location,
                                        std::string const& name);


/*!
    @ingroup    lue_cxx_api_hdf5_group
*/
class Dataset
{

public:

                   Dataset             (Identifier&& id);

                   Dataset             (Dataset const& other)=delete;

                   Dataset             (Dataset&& other)=default;

    virtual        ~Dataset            ()=default;

    Dataset&       operator=           (Dataset const& other)=delete;

    Dataset&       operator=           (Dataset&& other)=default;

    Identifier const& id               () const;

    Identifier     type_id             () const;

    Dataspace      dataspace           () const;

    void           resize              (std::vector<hsize_t> const&
                                            new_dimension_sizes);

    void           read                (hid_t const type_id,
                                        std::vector<hsize_t> const& start,
                                        std::vector<hsize_t> const& count,
                                        std::vector<hsize_t> const& stride,
                                        void* buffer) const;

    void           write               (hid_t const type_id,
                                        std::vector<hsize_t> const& start,
                                        std::vector<hsize_t> const& count,
                                        std::vector<hsize_t> const& stride,
                                        void const* buffer) const;

private:

    Identifier     _id;

};


Dataset            open_dataset        (hdf5::Identifier const& location,
                                        std::string const& name);

Dataset            create_dataset      (hdf5::Identifier const& location,
                                        std::string const& name,
                                        hid_t const datatype,
                                        Dataspace const& dataspace,
                                        hid_t const creation_property_list);

} // namespace hdf5
} // namespace lue
