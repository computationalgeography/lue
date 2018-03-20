#pragma once
#include "lue/domain.hpp"
#include "lue/properties.hpp"
#include "lue/property_set_configuration.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {

/*!
    @brief      Common property set class
    @sa         create_property_set(hdf5::Group const&, std::string const&,
                PropertySetConfiguration const&,
                Domain::Configuration const&)

    Property sets are aggregates of a domain and a collection of zero
    or more properties sharing that domain. This class contains the
    functionality common to all kinds of property sets. Property sets
    differ from each other depending on the properties of the time
    domain in the layered domain. More specialized property set APIs
    can be found in the lue::time namespace.
*/
class PropertySet:
    public hdf5::Group
{

public:

                   PropertySet         (hdf5::Group const& parent,
                                        std::string const& name);

                   PropertySet         (hdf5::Identifier const& id);

                   PropertySet         (hdf5::Group&& group);

                   PropertySet         (PropertySet const&)=delete;

                   PropertySet         (PropertySet&&)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const&)=delete;

    PropertySet&   operator=           (PropertySet&&)=default;

    PropertySetConfiguration const&
                   configuration       () const;

    Domain const&  domain              () const;

    Domain&        domain              ();

    Properties const& properties       () const;

    Properties&    properties          ();

private:

    PropertySetConfiguration _configuration;

    Domain         _domain;

    Properties     _properties;

};


PropertySet        create_property_set (hdf5::Group& parent,
                                        std::string const& name,
                                        PropertySetConfiguration const&
                                            configuration);

} // namespace lue
