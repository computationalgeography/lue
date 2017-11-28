#pragma once
#include "lue/property_sets.hpp"


namespace lue {

/*!
    @brief      TODO
    @sa         create_phenomenon(hdf5::Identifier const&, std::string const&)
*/
class Phenomenon:
    public hdf5::Group
{

public:

                   Phenomenon          (hdf5::Group const& parent,
                                        std::string const& name);

                   Phenomenon          (hdf5::Group&& group);

                   Phenomenon          (Phenomenon const&)=delete;

                   Phenomenon          (Phenomenon&&)=default;

                   ~Phenomenon         ()=default;

    Phenomenon&    operator=           (Phenomenon const&)=delete;

    Phenomenon&    operator=           (Phenomenon&&)=default;

    PropertySets const& property_sets  () const;

    PropertySets&  property_sets       ();

private:

    PropertySets   _property_sets;

};


Phenomenon         create_phenomenon   (hdf5::Group const& group,
                                        std::string const& name);

} // namespace lue
