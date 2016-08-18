#pragma once
#include "lue/cxx_api/property_sets.h"
#include <map>
#include <memory>
#include <string>
#include <vector>


namespace lue {

bool               phenomenon_exists   (hdf5::Identifier const& location,
                                        std::string const& name);


/*!
    @ingroup    lue_cxx_api_group
*/
class Phenomenon:
    public hdf5::Group
{

public:

                   Phenomenon          (hdf5::Identifier const& location,
                                        std::string const& name);

                   Phenomenon          (hdf5::Identifier&& location);

                   Phenomenon          (Phenomenon const& other)=delete;

                   Phenomenon          (Phenomenon&& other)=default;

                   ~Phenomenon         ()=default;

    Phenomenon&    operator=           (Phenomenon const& other)=delete;

    Phenomenon&    operator=           (Phenomenon&& other)=default;

    PropertySet&   add_property_set    (std::string const& name);

    PropertySet&   add_property_set    (std::string const& name,
                                        DomainConfiguration const&
                                            domain_configuration);

    PropertySets&  property_sets       () const;

private:

    std::unique_ptr<PropertySets> _property_sets;

};


Phenomenon         create_phenomenon   (hdf5::Identifier const& location,
                                        std::string const& name);

} // namespace lue
