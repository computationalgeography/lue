#pragma once
#include "lue/cxx_api/domain.h"
#include "lue/cxx_api/properties.h"
// #include "lue/cxx_api/time/omnipresent/property_set.h"
#include <memory>
#include <string>


namespace lue {

bool               property_set_exists (hdf5::Identifier const& location,
                                        std::string const& name);


/*!
    @ingroup    lue_cxx_api_group
*/
class PropertySet:
    public hdf5::Group
{

public:

                   PropertySet         (hdf5::Identifier const& location,
                                        std::string const& name);

                   PropertySet         (hdf5::Identifier&& location);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

    Domain&        domain              () const;

    Property&      add_property        (std::string const& name);

    Properties&    properties          () const;

private:

    std::unique_ptr<Domain> _domain;

    std::unique_ptr<Properties> _properties;

};


PropertySet        create_property_set (hdf5::Identifier const& location,
                                        std::string const& name,
                                        DomainConfiguration const&
                                            domain_configuration);

} // namespace lue
