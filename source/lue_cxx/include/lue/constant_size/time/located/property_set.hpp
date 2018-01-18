#pragma once
#include "lue/constant_size/property_set.hpp"
#include "lue/constant_size/time/located/domain.hpp"
#include "lue/property_sets.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

class PropertySet:
    public constant_size::PropertySet
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

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
