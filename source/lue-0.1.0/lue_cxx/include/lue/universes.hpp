#pragma once
#include "lue/collection.hpp"
#include "lue/universe.hpp"


namespace lue {

/*!
    @brief      Collection of universes
    @sa         create_universes(hdf5::Identifier const&)

    The collection is modeled using an HDF5 group.
*/
class Universes:
    public Collection<Universe>
{

public:

                   Universes           (hdf5::Group const& parent);

                   Universes           (Collection<Universe>&& collection);

                   Universes           (Universes const&)=delete;

                   Universes           (Universes&&)=default;

                   ~Universes          ()=default;

    Universes&     operator=           (Universes const&)=delete;

    Universes&     operator=           (Universes&&)=default;

    Universe&      add                 (std::string const& name);

private:

};


Universes          create_universes    (hdf5::Group const& group);

} // namespace lue
