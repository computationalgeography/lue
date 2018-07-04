#pragma once
#include "lue/object/property/property_set.hpp"
#include "lue/core/collection.hpp"


namespace lue {

class PropertySets:
    public Collection<PropertySet>
{

public:

    explicit       PropertySets        (hdf5::Group& parent);

                   PropertySets        (Collection<PropertySet>&& collection);

                   PropertySets        (PropertySets const&)=delete;

                   PropertySets        (PropertySets&&)=default;

                   ~PropertySets       ()=default;

    PropertySets&  operator=           (PropertySets const&)=delete;

    PropertySets&  operator=           (PropertySets&&)=default;

    PropertySet&   add                 (std::string const& name);

    PropertySet&   add                 (std::string const& name,
                                        TimeConfiguration const&
                                            time_configuration,
                                        SpaceConfiguration const&
                                            space_configuration);

private:

};


PropertySets       create_property_sets(hdf5::Group& parent);

} // namespace lue
