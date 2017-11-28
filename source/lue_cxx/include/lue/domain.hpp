#pragma once
#include "lue/hdf5/group.hpp"


namespace lue {

/*!
    @brief      TODO
    @sa         create_domain(hdf5::Identifier const&,
                Domain::Configuration const&)
*/
class Domain:
    public hdf5::Group
{

public:

    class Configuration
    {

    public:

        enum class DomainType
        {

            omnipresent,

            located

        };

                   Configuration       (DomainType const domain_type);

                   Configuration       (hdf5::Attributes const& attributes);

                   Configuration       (Configuration const& other)=default;

                   ~Configuration      ()=default;

        Configuration& operator=       (Configuration const& other)=default;

        DomainType domain_type         () const;

        void       save                (hdf5::Attributes& attributes) const;

    private:

        DomainType _domain_type;

        void       load                (hdf5::Attributes const& attributes);

    };


                   Domain              (hdf5::Group const& parent);

                   Domain              (hdf5::Group&& group);

                   Domain              (Domain const&)=delete;

                   Domain              (Domain&&)=default;

    virtual        ~Domain             ()=default;

    Domain&        operator=           (Domain const&)=delete;

    Domain&        operator=           (Domain&&)=default;

    Configuration const&
                   configuration       () const;

private:

    Configuration  _configuration;

};


Domain             create_domain       (hdf5::Group const& parent,
                                        Domain::Configuration const&
                                            configuration);

} // namespace lue
