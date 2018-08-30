#pragma once
#include "lue/hdf5.hpp"
#include "lue/core/define.hpp"


namespace lue {

class PropertyGroup:
    public hdf5::Group
{

public:

                   PropertyGroup       (hdf5::Group& parent,
                                        std::string const& name);

                   PropertyGroup       (hdf5::Group&& group);

                   PropertyGroup       (PropertyGroup const&)=delete;

                   PropertyGroup       (PropertyGroup&&)=default;

                   ~PropertyGroup      ()=default;

    PropertyGroup& operator=           (PropertyGroup const&)=delete;

    PropertyGroup& operator=           (PropertyGroup&&)=default;

    std::string    name                () const;

    bool           time_is_discretized () const;

    bool           space_is_discretized() const;

    TimeDiscretization
                   time_discretization_type() const;

    SpaceDiscretization
                   space_discretization_type() const;

    void           set_time_discretization(
                                        TimeDiscretization type,
                                        PropertyGroup& property);

    void           set_space_discretization(
                                        SpaceDiscretization type,
                                        PropertyGroup& property);

    PropertyGroup  time_discretization_property();

    PropertyGroup  space_discretization_property();

    hdf5::Group    property_set_group  ();

private:

};


PropertyGroup      create_property_group(
                                        hdf5::Group& parent,
                                        std::string const& name);

}  // namespace lue
