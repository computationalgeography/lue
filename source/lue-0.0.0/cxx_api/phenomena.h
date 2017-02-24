#pragma once
#include "lue/cxx_api/collection.h"
#include "lue/cxx_api/phenomenon.h"


namespace lue {

bool               phenomena_exists    (hdf5::Identifier const& location);


/*!
    @ingroup    lue_cxx_api_group
    @brief      Collection for phenomena
*/
class Phenomena:
    public Collection<Phenomenon>
{

public:

                   Phenomena           (hdf5::Identifier const& location);

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
