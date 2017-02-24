#pragma once
#include "lue/property_set.h"


namespace lue {
namespace constant_size {

class PropertySet:
    public lue::PropertySet
{

public:

//                    PropertySet         (Phenomenon& phenomenon,
//                                         std::string const& name);

                   PropertySet         (hdf5::Identifier const& id);

                   PropertySet         (lue::PropertySet&& property_set);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

private:

};


PropertySet        create_property_set (hdf5::Group& group,
                                        std::string const& name);

}  // namespace constant_size
}  // namespace lue
