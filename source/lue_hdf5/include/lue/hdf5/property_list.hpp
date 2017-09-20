#pragma once
#include "lue/hdf5/identifier.hpp"


namespace lue {
namespace hdf5 {

/*!
    @brief      This class represents an HDF5 property list.
    @sa         [Property list interface](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#FileAccessPropFuncs)
*/
class PropertyList
{

public:

    explicit       PropertyList        (hid_t const class_id);

                   PropertyList        (PropertyList const& other)=delete;

                   PropertyList        (PropertyList&& other)=default;

                   ~PropertyList       ()=default;

    PropertyList&  operator=           (PropertyList const& other)=delete;

    PropertyList&  operator=           (PropertyList&& other)=default;

    Identifier const& id               () const;

private:

    Identifier     _id;

};

} // namespace hdf5
} // namespace lue
