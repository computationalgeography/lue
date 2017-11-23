#pragma once
#include "lue/property.hpp"


namespace lue {
namespace constant_size {

class Property:
    public lue::Property
{

public:

                   Property            (hdf5::Identifier const& id);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

private:

};


Property           create_property     (hdf5::Group& group,
                                        std::string const& name,
                                        Property::Configuration const&
                                            configuration);

}  // namespace constant_size
}  // namespace lue
