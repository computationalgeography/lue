#pragma once
#include "lue/collection.h"
#include "lue/phenomenon.h"


namespace lue {

// bool               phenomena_exists    (hdf5::Identifier const& location);


/*!
    @brief      Collection for phenomena
*/
class Phenomena:
    public Collection<Phenomenon>
{

public:

                   Phenomena           (hdf5::Identifier const& location);

                   Phenomena           (hdf5::Identifier&& id);

                   Phenomena           (Collection<Phenomenon>&& collection);

                   Phenomena           (Phenomena const& other)=delete;

                   Phenomena           (Phenomena&& other)=default;

                   ~Phenomena          ()=default;

    Phenomena&     operator=           (Phenomena const& other)=delete;

    Phenomena&     operator=           (Phenomena&& other)=default;

    Phenomenon&    add                 (std::string const& name);

private:

};


Phenomena          create_phenomena    (hdf5::Identifier const& location);

} // namespace lue
