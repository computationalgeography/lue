#pragma once
#include "lue/constant_size/time/omnipresent/property_set.h"
#include "lue/constant_size/property.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class Property:
    public constant_size::Property
{

public:

                   Property            (constant_size::Property&& property);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

};


Property           create_property     (PropertySet& property_set,
                                        std::string const& name,
                                        Property::Configuration const&
                                            configuration);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
