#pragma once
#include "lue/property.hpp"


namespace lue {
namespace constant_collection {

class Property:
    public lue::Property
{

public:

                   Property            (hdf5::Group&& group);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

private:

};


Property           create_property     (hdf5::Group& group,
                                        std::string const& name,
                                        Configuration const& configuration);

}  // namespace constant_collection
}  // namespace lue
