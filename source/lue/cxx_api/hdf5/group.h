#pragma once
#include "lue/cxx_api/hdf5/attributes.h"
#include <string>


namespace lue {
namespace hdf5 {

/*!
    @ingroup    lue_cxx_api_hdf5_group
    @brief      This class represents an HDF5 group.
*/
class Group
{

public:

                   Group               (Identifier&& id);

                   Group               (Group const& other)=delete;

                   Group               (Group&& other);

    virtual        ~Group              ()=default;

    Group&         operator=           (Group const& other)=delete;

    Group&         operator=           (Group&& other);

    Identifier const& id               () const;

    Attributes const& attributes       () const;

private:

    Identifier     _id;

    Attributes     _attributes;

};

} // namespace hdf5
} // namespace lue
