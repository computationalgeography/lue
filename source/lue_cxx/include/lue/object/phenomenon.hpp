#pragma once
#include "lue/object/property_sets.hpp"


namespace lue {

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

private:

    PropertySets   _property_sets;

};


Phenomenon         create_phenomenon   (hdf5::Group& parent,
                                        std::string const& name);

} // namespace lue
