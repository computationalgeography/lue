#pragma once
#include "lue/cxx_api/collection.h"
#include "lue/cxx_api/property_set.h"


namespace lue {

bool               property_sets_exists(hdf5::Identifier const& location);


/*!
    @ingroup    lue_cxx_api_group
*/
class PropertySets:
    public Collection<PropertySet>
{

public:

                   PropertySets        (hdf5::Identifier const& location);

                   PropertySets        (PropertySets const& other)=delete;

                   PropertySets        (PropertySets&& other)=default;

                   ~PropertySets       ()=default;

    PropertySets&  operator=           (PropertySets const& other)=delete;

    PropertySets&  operator=           (PropertySets&& other)=default;

    PropertySet&   add                 (std::string const& name);

    PropertySet&   add                 (std::string const& name,
                                        DomainConfiguration const&
                                            domain_configuration);

private:

};


PropertySets       create_property_sets(hdf5::Identifier const& location);

} // namespace lue
