#pragma once
#include "lue/cxx_api/collection.h"
#include "lue/cxx_api/property.h"


namespace lue {

bool               properties_exists   (hdf5::Identifier const& location);


/*!
    @ingroup    lue_cxx_api_group
    @brief      Collection of properties

    The collection is modeled using an HDF5 group.
*/
class Properties:
    public Collection<Property>
{

public:

                   Properties          (hdf5::Identifier const& location);

                   Properties          (Properties const& other)=delete;

                   Properties          (Properties&& other)=default;

                   ~Properties         ()=default;

    Properties&    operator=           (Properties const& other)=delete;

    Properties&    operator=           (Properties&& other)=default;

    Property&      add                 (std::string const& name);

private:

};


Properties         create_properties   (hdf5::Identifier const& location);

} // namespace lue
