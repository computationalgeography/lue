#pragma once
#include "lue/collection.hpp"
#include "lue/phenomenon.hpp"


namespace lue {

/*!
    @brief      Collection for phenomena
    @sa         create_phenomena(hdf5::Identifier const&)
*/
class Phenomena:
    public Collection<Phenomenon>
{

public:

                   Phenomena           (hdf5::Group const& parent);

                   Phenomena           (Collection<Phenomenon>&& collection);

                   Phenomena           (Phenomena const&)=delete;

                   Phenomena           (Phenomena&&)=default;

                   ~Phenomena          ()=default;

    Phenomena&     operator=           (Phenomena const&)=delete;

    Phenomena&     operator=           (Phenomena&&)=default;

    Phenomenon&    add                 (std::string const& name);

private:

};


Phenomena          create_phenomena    (hdf5::Group const& group);

} // namespace lue
