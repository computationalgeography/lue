#pragma once
#include "lue/hdf5.hpp"


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

protected:

                   PropertyGroup       (hdf5::Group& parent,
                                        std::string const& name);

private:

};


PropertyGroup      create_property_group(
                                        hdf5::Group& parent,
                                        std::string const& name);

}  // namespace lue
