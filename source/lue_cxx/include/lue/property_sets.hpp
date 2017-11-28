#pragma once
#include "lue/collection.hpp"
#include "lue/property_set.hpp"


namespace lue {

/*!
    @brief      TODO
    @sa         create_property_sets(hdf5::Group const&)
*/
class PropertySets:
    public Collection<PropertySet>
{

public:

                   PropertySets        (hdf5::Group const& parent);

                   PropertySets        (Collection<PropertySet>&& collection);

                   PropertySets        (PropertySets const&)=delete;

                   PropertySets        (PropertySets&&)=default;

                   ~PropertySets       ()=default;

    PropertySets&  operator=           (PropertySets const&)=delete;

    PropertySets&  operator=           (PropertySets&&)=default;

    // PropertySet&   add                 (std::string const& name);

private:

};


PropertySets       create_property_sets(hdf5::Group const& group);

} // namespace lue
