#pragma once
#include "lue/constant_collection/time/omnipresent/same_shape/value.hpp"
#include "lue/constant_collection/time/omnipresent/domain.hpp"
#include "lue/constant_collection/property_set.hpp"
#include "lue/phenomenon.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

/*!
    @brief      TODO
    @sa         create_property_set(Phenomenon&, std::string const&),
                create_property_set(Phenomenon&, std::string const&,
                PropertySet::Ids const&)
*/
class PropertySet:
    public constant_collection::PropertySet
{

public:

                   PropertySet         (hdf5::Identifier const& id);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

    Domain const&  domain              () const;

    Domain&        domain              ();

private:

    Domain         _domain;

};


PropertySet        create_property_set (Phenomenon& phenomenon,
                                        std::string const& name);

PropertySet        create_property_set (Phenomenon& phenomenon,
                                        std::string const& name,
                                        PropertySet::Ids const& ids);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
