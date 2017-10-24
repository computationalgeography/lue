#pragma once
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/constant_size/property.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class Property:
    public constant_size::Property
{

public:

                   // Property            (constant_size::Property&& property);

                   Property            (hdf5::Identifier const& id);

                   Property            (lue::Property const& property);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

};


Property           create_property     (hdf5::Group& group,
                                        std::string const& name,
                                        Property::Configuration const&
                                            configuration);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
