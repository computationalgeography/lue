#pragma once
#include "lue/constant_size/time/located/property.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace constant_shape {

class Property:
    public located::Property
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


Property        create_property        (hdf5::Group& group,
                                        std::string const& name,
                                        Configuration const& configuration);

}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
