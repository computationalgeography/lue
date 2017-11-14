#pragma once
#include "lue/constant_size/property.hpp"
#include "lue/time/property_configuration.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

class Property:
    public constant_size::Property
{

public:

                   Property            (hdf5::Identifier const& location);

                   Property            (constant_size::Property&& property);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

    lue::time::PropertyConfiguration const&
                   configuration2      () const;

    void           discretize_time     (lue::Property const& property);

    bool           time_is_discretized () const;

    lue::Property  time_discretization () const;

private:

    lue::time::PropertyConfiguration _configuration;

};


Property           create_property     (hdf5::Group& group,
                                        std::string const& name,
                                        Property::Configuration const&
                                            configuration,
                                        lue::time::PropertyConfiguration const&
                                            property_configuration);

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
