#pragma once
#include "lue/object/property_sets.hpp"


namespace lue {

class Phenomenon:
    public hdf5::Group
{

public:

                   Phenomenon          (hdf5::Group& parent,
                                        std::string const& name);

                   Phenomenon          (hdf5::Group&& group);

                   Phenomenon          (Phenomenon const&)=delete;

                   Phenomenon          (Phenomenon&&)=default;

                   ~Phenomenon         ()=default;

    Phenomenon&    operator=           (Phenomenon const&)=delete;

    Phenomenon&    operator=           (Phenomenon&&)=default;

    PropertySets const& collection_property_sets() const;

    PropertySets&  collection_property_sets();

    PropertySets const& property_sets  () const;

    PropertySets&  property_sets       ();

private:

    //! Sets of properties of the collection of objects
    PropertySets   _collection_property_sets;

    //! Sets of properties of the objects in the collection
    PropertySets   _property_sets;

};


Phenomenon         create_phenomenon   (hdf5::Group& parent,
                                        std::string const& name);

} // namespace lue
