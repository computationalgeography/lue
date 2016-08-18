#pragma once
#include "lue/cxx_api/collection.h"
#include "lue/cxx_api/universe.h"


namespace lue {

bool               universes_exists    (hdf5::Identifier const& location);


/*!
    @ingroup    lue_cxx_api_group
    @brief      Collection of universes

    The collection is modeled using an HDF5 group.
*/
class Universes:
    public Collection<Universe>
{

public:

                   Universes           (hdf5::Identifier const& location);

                   Universes           (Universes const& other)=delete;

                   Universes           (Universes&& other)=default;

                   ~Universes          ()=default;

    Universes&     operator=           (Universes const& other)=delete;

    Universes&     operator=           (Universes&& other)=default;

    Universe&      add                 (std::string const& name);

private:

};


Universes          create_universes    (hdf5::Identifier const& location);

} // namespace lue
