#pragma once
#include "lue/constant_size/property.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

class Property:
    public constant_size::Property
{

public:

                   Property            (constant_size::Property&& property);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    void           discretize_time     (lue::Property const& property);

    bool           time_is_discretized () const;

    lue::Property  time_discretization () const;

private:

};


Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        Configuration const& configuration);

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
