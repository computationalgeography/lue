#pragma once
#include "lue/cxx_api/hdf5/attributes.h"
#include <string>


namespace lue {
namespace hdf5 {

bool               group_exists        (hdf5::Identifier const& location,
                                        std::string const& name);


/*!
    @ingroup    lue_cxx_api_hdf5_group
    @brief      This class represents an HDF5 group.
*/
class Group
{

public:

                   Group               (Identifier const& location,
                                        std::string const& name);

                   Group               (Identifier&& id);

                   Group               (Group const& other)=delete;

                   Group               (Group&& other);

    virtual        ~Group              ()=default;

    Group&         operator=           (Group const& other)=delete;

    Group&         operator=           (Group&& other);

    Identifier const& id               () const;

    std::string    name                () const;

    Attributes const& attributes       () const;

    Attributes&    attributes          ();

    void           create_soft_link    (hdf5::Identifier const& location,
                                        std::string const& name);

private:

    Identifier     _id;

    Attributes     _attributes;

};


Group              create_group        (hdf5::Identifier const& location,
                                        std::string const& name);

} // namespace hdf5
} // namespace lue
