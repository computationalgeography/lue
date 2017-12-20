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

                   PropertySet         (hdf5::Group&& group);

                   PropertySet         (PropertySet const&)=delete;

                   PropertySet         (PropertySet&&)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const&)=delete;

    PropertySet&   operator=           (PropertySet&&)=default;

    Domain const&  domain              () const;

    Domain&        domain              ();

private:

    Domain         _domain;

};


PropertySet        create_property_set (PropertySets& property_sets,
                                        std::string const& name);

PropertySet        create_property_set (PropertySets& property_sets,
                                        std::string const& name,
                                        PropertySet::Ids const& ids);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
