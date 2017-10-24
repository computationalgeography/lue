#pragma once
#include "lue/constant_size/time/located/property.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {

class Property:
    public located::Property
{

public:

                   Property            (hdf5::Identifier const& id);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

private:

};


Property        create_property        (hdf5::Group& group,
                                        std::string const& name,
                                        Property::Configuration const&
                                            configuration);

}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
