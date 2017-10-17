#pragma once
#include "lue/constant_size/time/located/property_set.hpp"
#include "lue/constant_size/time/located/shared/domain.hpp"
#include "lue/property_sets.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

class PropertySet:
    public located::PropertySet
{

public:

                   PropertySet         (hdf5::Identifier const& id);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

    // Domain const&  domain              () const;

    // Domain&        domain              ();

private:

    // Domain         _domain;

};


PropertySet        create_property_set (PropertySets& property_sets,
                                        std::string const& name);

// PropertySet        create_property_set (PropertySets& property_sets,
//                                         std::string const& name,
//                                         same_shape::Value const& ids);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
