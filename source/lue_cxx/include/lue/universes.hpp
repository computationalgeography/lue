#pragma once
#include "lue/collection.hpp"
#include "lue/universe.hpp"


namespace lue {

// bool               universes_exists    (hdf5::Identifier const& location);


/*!
    @brief      Collection of universes

    The collection is modeled using an HDF5 group.
*/
class Universes:
    public Collection<Universe>
{

public:

                   Universes           (hdf5::Identifier const& location);

                   Universes           (hdf5::Identifier&& id);

                   Universes           (Collection<Universe>&& collection);

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
