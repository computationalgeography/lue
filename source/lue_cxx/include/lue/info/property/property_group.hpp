#pragma once
#include "lue/hdf5.hpp"
#include "lue/core/define.hpp"


namespace lue {

class PropertyGroup:
    public hdf5::Group
{

public:

                   PropertyGroup       (hdf5::Group&& group);

                   PropertyGroup       (PropertyGroup const&)=delete;

                   PropertyGroup       (PropertyGroup&&)=default;

                   ~PropertyGroup      ()=default;

    PropertyGroup& operator=           (PropertyGroup const&)=delete;

    PropertyGroup& operator=           (PropertyGroup&&)=default;

    std::string    name                () const;

    bool           space_is_discretized() const;

    SpaceDiscretization
                   space_discretization_type() const;

    void           set_space_discretisation(
                                    SpaceDiscretization type,
                                    PropertyGroup& property);

    PropertyGroup  space_discretization_property();

    virtual hdf5::Group property_set_group();

protected:

                   PropertyGroup       (hdf5::Group& parent,
                                        std::string const& name);

private:

};


PropertyGroup      create_property_group(
                                        hdf5::Group& parent,
                                        std::string const& name);

}  // namespace lue
