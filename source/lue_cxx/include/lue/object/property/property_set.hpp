#pragma once
#include "lue/object/identity/object_tracker.hpp"
#include "lue/object/property/properties.hpp"
#include "lue/object/space/space_domain.hpp"
#include "lue/object/time/time_domain.hpp"
#include <memory>


namespace lue {

class PropertySet:
    public hdf5::Group
{

public:

                   PropertySet         (hdf5::Group& parent,
                                        std::string const& name);

                   PropertySet         (hdf5::Group&& group);

                   PropertySet         (PropertySet const&)=delete;

                   PropertySet         (PropertySet&&)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const&)=delete;

    PropertySet&   operator=           (PropertySet&&)=default;

    ObjectTracker const&
                   object_tracker      () const;

    ObjectTracker& object_tracker      ();

    bool           has_time_domain     () const;

    bool           has_space_domain    () const;

    TimeDomain const& time_domain      () const;

    SpaceDomain const& space_domain    () const;

    Properties const& properties       () const;

    same_shape::Property& add_property (std::string const& name,
                                        hdf5::Datatype const& datatype);

private:

    ObjectTracker  _object_tracker;

    std::unique_ptr<TimeDomain> _time_domain;

    std::unique_ptr<SpaceDomain> _space_domain;

    Properties     _properties;

};


PropertySet        create_property_set (hdf5::Group& parent,
                                        std::string const& name);

PropertySet        create_property_set (hdf5::Group& parent,
                                        std::string const& name,
                                        TimeConfiguration const&
                                            time_configuration,
                                        SpaceConfiguration const&
                                            space_configuration);

}  // namespace lue
