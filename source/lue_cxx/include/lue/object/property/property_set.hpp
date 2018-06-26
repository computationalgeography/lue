#pragma once
#include "lue/object/property/properties.hpp"
#include "lue/object/space/space_domain.hpp"
#include "lue/object/time/time_domain.hpp"


namespace lue {

class PropertySet:
    public hdf5::Group
{

public:

                   PropertySet         (hdf5::Group const& parent,
                                        std::string const& name);

                   PropertySet         (hdf5::Group&& group);

                   PropertySet         (PropertySet const&)=delete;

                   PropertySet         (PropertySet&&)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const&)=delete;

    PropertySet&   operator=           (PropertySet&&)=default;

    TimeDomain const& time_domain      () const;

    SpaceDomain const& space_domain    () const;

    Properties const& properties       () const;

private:

    TimeDomain     _time_domain;

    SpaceDomain    _space_domain;

    Properties     _properties;

};


PropertySet        create_property_set (hdf5::Group& parent,
                                        std::string const& name,
                                        TimeConfiguration const&
                                            time_configuration,
                                        SpaceConfiguration const&
                                            space_configuration);

}  // namespace lue
