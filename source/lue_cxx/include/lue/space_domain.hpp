#pragma once
#include "lue/hdf5/group.hpp"


namespace lue {

/*!
    @sa         create_space_domain(hdf5::Group const&,
                SpaceDomain::Configuration const&),
                space_domain_exists(hdf5::Group const&)
*/
class SpaceDomain:
    public hdf5::Group
{

public:

    class Configuration
    {

    public:

        enum class DomainType
        {

            located

        };

        enum class ItemType
        {

            box,

            point

        };

                   Configuration       (DomainType const domain_type,
                                        ItemType const item_type);

                   Configuration       (hdf5::Attributes const& attributes);

                   Configuration       (Configuration const& other)=default;

                   ~Configuration      ()=default;

        Configuration& operator=       (Configuration const& other)=default;

        DomainType domain_type         () const;

        ItemType   item_type           () const;

        void       save                (hdf5::Attributes& attributes) const;

    private:

        DomainType _domain_type;

        ItemType   _item_type;

        void       load                (hdf5::Attributes const& attributes);

    };


                   SpaceDomain         (hdf5::Group const& parent);

                   SpaceDomain         (hdf5::Group&& group);

                   SpaceDomain         (SpaceDomain const&)=delete;

                   SpaceDomain         (SpaceDomain&&)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const&)=delete;

    SpaceDomain&   operator=           (SpaceDomain&&)=default;

    Configuration const&
                   configuration       () const;

private:

    Configuration  _configuration;

};


SpaceDomain        create_space_domain (hdf5::Group const& parent,
                                        SpaceDomain::Configuration const&
                                            configuration);

bool               space_domain_exists (hdf5::Group const& parent);

}  // namespace lue
