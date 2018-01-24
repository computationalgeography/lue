#pragma once
#include "lue/constant_collection/time/omnipresent/property_set.hpp"
#include "lue/constant_collection/property.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

class Property:
    public constant_collection::Property
{

public:

    explicit       Property            (hdf5::Group&& group);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

};


Property           create_property     (hdf5::Group& group,
                                        std::string const& name,
                                        Configuration const& configuration);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
