#pragma once
#include "lue/hdf5/group.hpp"


namespace lue {

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

            box

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


                   SpaceDomain         (hdf5::Group const& group);

                   SpaceDomain         (hdf5::Group&& group);

                   SpaceDomain         (SpaceDomain const& other)=default;

                   SpaceDomain         (SpaceDomain&& other)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const& other)=default;

    SpaceDomain&   operator=           (SpaceDomain&& other)=default;

    Configuration const&
                   configuration       () const;

private:

    Configuration  _configuration;

};


SpaceDomain        create_space_domain (hdf5::Group const& group,
                                        SpaceDomain::Configuration const&
                                            configuration);

bool               space_domain_exists (hdf5::Group const& group);

}  // namespace lue
