#pragma once
#include "lue/object/property/property_set.hpp"
#include "lue/core/collection.hpp"


namespace lue {
namespace data_model {

class PropertySets:
    public Collection<PropertySet>
{

public:

                   PropertySets        (hdf5::Group& parent,
                                        std::string const& name);

    explicit       PropertySets        (Collection<PropertySet>&& collection);

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
                                        Clock const& clock);

    PropertySet&   add                 (std::string const& name,
                                        TimeDomain& domain);

    PropertySet&   add                 (std::string const& name,
                                        TimeConfiguration const&
                                            time_configuration,
                                        Clock const& clock,
                                        ObjectTracker& object_tracker);

    PropertySet&   add                 (std::string const& name,
                                        TimeDomain& domain,
                                        ObjectTracker& object_tracker);

    PropertySet&   add                 (std::string const& name,
                                        TimeConfiguration const&
                                            time_configuration,
                                        Clock const& clock,
                                        SpaceConfiguration const&
                                            space_configuration,
                                        hdf5::Datatype const&
                                            space_coordinate_datatype,
                                        std::size_t rank);

    PropertySet&   add                 (std::string const& name,
                                        TimeDomain& time_domain,
                                        SpaceConfiguration const&
                                            space_configuration,
                                        hdf5::Datatype const&
                                            space_coordinate_datatype,
                                        std::size_t rank);

    PropertySet&   add                 (std::string const& name,
                                        TimeDomain& time_domain,
                                        ObjectTracker& object_tracker,
                                        SpaceConfiguration const&
                                            space_configuration,
                                        hdf5::Datatype const&
                                            space_coordinate_datatype,
                                        std::size_t rank);

private:

};


PropertySets       create_property_sets(hdf5::Group& parent,
                                        std::string const& name);

} // namespace data_model
} // namespace lue
