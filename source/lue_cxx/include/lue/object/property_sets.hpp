#pragma once
#include "lue/object/property/property_set.hpp"
#include "lue/core/collection.hpp"


namespace lue {

class PropertySets:
    public Collection<PropertySet>
{

public:

                   PropertySets        (hdf5::Group& parent,
                                        std::string const& name);

                   PropertySets        (Collection<PropertySet>&& collection);

                   PropertySets        (PropertySets const&)=delete;

                   PropertySets        (PropertySets&&)=default;

                   ~PropertySets       ()=default;

    PropertySets&  operator=           (PropertySets const&)=delete;

    PropertySets&  operator=           (PropertySets&&)=default;

    PropertySet&   add                 (std::string const& name);

    PropertySet&   add                 (std::string const& name,
                                        SpaceConfiguration const&
                                            space_configuration,
                                        hdf5::Datatype const&
                                            space_coordinate_datatype,
                                        std::size_t rank);

    PropertySet&   add                 (std::string const& name,
                                        TimeConfiguration const&
                                            time_configuration,
                                        SpaceConfiguration const&
                                            space_configuration,
                                        hdf5::Datatype const&
                                            space_coordinate_datatype,
                                        std::size_t rank);

private:

};


PropertySets       create_property_sets(hdf5::Group& parent,
                                        std::string const& name);

} // namespace lue
