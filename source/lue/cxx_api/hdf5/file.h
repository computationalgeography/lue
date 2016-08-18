#pragma once
#include "lue/cxx_api/hdf5/identifier.h"


namespace lue {
namespace hdf5 {

/*!
    @ingroup    lue_cxx_api_hdf5_group
    @brief      This class represents an HDF5 file.
*/
class File
{

public:

                   File                (Identifier&& id);

                   File                (File const& other)=delete;

                   File                (File&& other);

    virtual        ~File               ()=default;

    File&          operator=           (File const& other)=delete;

    File&          operator=           (File&& other);

    Identifier const& id               () const;

private:

    Identifier     _id;

};

} // namespace hdf5
} // namespace lue
