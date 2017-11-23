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

                   Property            (hdf5::Identifier const& id);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

};


Property           create_property     (hdf5::Group& group,
                                        std::string const& name,
                                        Property::Configuration const&
                                            configuration);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
