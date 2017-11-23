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

                   PropertyList        (PropertyList const&)=delete;

                   PropertyList        (PropertyList&&)=default;

    virtual        ~PropertyList       ()=default;

    PropertyList&  operator=           (PropertyList const&)=delete;

    PropertyList&  operator=           (PropertyList&&)=default;

    Identifier const& id               () const;

protected:

    explicit       PropertyList        (hid_t class_id);

private:

    Identifier     _id;

};

} // namespace hdf5
} // namespace lue
