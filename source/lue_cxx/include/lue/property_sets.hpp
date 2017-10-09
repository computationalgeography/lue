#pragma once
#include "lue/collection.hpp"
#include "lue/property_set.hpp"


namespace lue {

// bool               property_sets_exists(hdf5::Identifier const& location);


/*!
    @brief      TODO
    @sa         create_property_sets(hdf5::Identifier const&)
*/
class PropertySets:
    public Collection<PropertySet>
{

public:

                   PropertySets        (hdf5::Identifier const& location);

                   PropertySets        (Collection<PropertySet>&& collection);

                   PropertySets        (PropertySets const& other)=delete;

                   PropertySets        (PropertySets&& other)=default;

                   ~PropertySets       ()=default;

    PropertySets&  operator=           (PropertySets const& other)=delete;

    PropertySets&  operator=           (PropertySets&& other)=default;

    // PropertySet&   add                 (std::string const& name);

    // PropertySet&   add                 (std::string const& name,
    //                                     PropertySetConfiguration const&
    //                                         configuration,
    //                                     DomainConfiguration const&
    //                                         domain_configuration);

private:

};


PropertySets       create_property_sets(hdf5::Identifier const& location);

} // namespace lue
