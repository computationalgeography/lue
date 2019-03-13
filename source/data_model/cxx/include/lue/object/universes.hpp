#pragma once
#include "lue/object/universe.hpp"
#include "lue/core/collection.hpp"


namespace lue {

class Universes:
    public Collection<Universe>
{

public:

                   Universes           (hdf5::Group& parent);

                   Universes           (Collection<Universe>&& collection);

                   Universes           (Universes const&)=delete;

                   Universes           (Universes&&)=default;

                   ~Universes          ()=default;

    Universes&     operator=           (Universes const&)=delete;

    Universes&     operator=           (Universes&&)=default;

    Universe&      add                 (std::string const& name);

private:

};


Universes          create_universes    (hdf5::Group& parent);

} // namespace lue
