#pragma once
#include "lue/cxx_api/hdf5/group.h"


namespace lue {

bool               property_exists     (hdf5::Identifier const& location,
                                        std::string const& name);


/*!
    @ingroup    lue_cxx_api_group
*/
class Property:
    public hdf5::Group
{

public:

                   Property            (hdf5::Identifier const& location,
                                        std::string const& name);

                   Property            (hdf5::Identifier&& location);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

private:

};


Property           create_property     (hdf5::Identifier const& location,
                                        std::string const& name);

} // namespace lue
