#pragma once
#include "lue/domain.hpp"
#include "lue/properties.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {

/*!
    @brief      Common property set class
    @sa         create_property_set(hdf5::Group const&, std::string const&,
                PropertySet::Configuration const&,
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

    class Configuration
    {

    public:

                   Configuration       (CollectionVariability const type);

                   Configuration       (hdf5::Attributes const& attributes);

                   ~Configuration      ()=default;

        CollectionVariability
                   size_of_item_collection_type() const;

        void       save                (hdf5::Attributes& attributes) const;

    private:

        CollectionVariability
                   _size_of_item_collection_type;

        void       load                (hdf5::Attributes const& attributes);

    };

                   PropertySet         (hdf5::Group const& parent,
                                        std::string const& name);

                   // PropertySet         (hdf5::Identifier const& id);

                   // PropertySet         (hdf5::Identifier&& location);

                   PropertySet         (hdf5::Group&& group);

                   PropertySet         (PropertySet const&)=delete;

                   PropertySet         (PropertySet&&)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const&)=delete;

    PropertySet&   operator=           (PropertySet&&)=default;

    Configuration const&
                   configuration       () const;

    // Domain&        domain              () const;

    // Property&      add_property        (std::string const& name,
    //                                     ValueConfiguration const&
    //                                         value_configuration);

    // Properties<Property>&
    //                properties          () const;

    Domain const&  domain              () const;

    Properties const& properties       () const;

    Properties&    properties          ();

private:

    Configuration  _configuration;

    Domain         _domain;

    Properties     _properties;

    // //! The domain shared by the properties in the collection
    // std::unique_ptr<Domain> _domain;

    // //! The collection of properties sharing a domain
    // std::unique_ptr<Properties<Property>> _properties;

};


PropertySet        create_property_set (hdf5::Group const& group,
                                        std::string const& name,
                                        PropertySet::Configuration const&
                                            configuration,
                                        Domain::Configuration const&
                                            domain_configuration);

// PropertySet        create_property_set (hdf5::Identifier const& location,
//                                         std::string const& name);
//                                         // PropertySetConfiguration const&
//                                         //     configuration,
//                                         // DomainConfiguration const&
//                                         //     domain_configuration);

} // namespace lue
