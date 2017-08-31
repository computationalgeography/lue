#pragma once
#include "lue/property.hpp"


namespace lue {
namespace constant_size {

class Property:
    public lue::Property
{

public:

                   Property            (lue::Property&& property);

                   Property            (hdf5::Identifier const& id);

                   Property            (lue::Property const& property);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

private:

};


Property           create_property     (hdf5::Group& group,
                                        std::string const& name,
                                        Property::Configuration const&
                                            configuration);

}  // namespace constant_size
}  // namespace lue
