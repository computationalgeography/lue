#pragma once
#include "lue/define.hpp"
#include "lue/hdf5/group.hpp"
// #include "lue/cxx_api/domain_configuration.h"
// #include "lue/cxx_api/space_domain.h"
// #include "lue/cxx_api/time_domain.h"
// #include <memory>


namespace lue {

// bool               domain_exists       (hdf5::Identifier const& location);


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

                   Configuration       (TimeDomainType const type);

                   Configuration       (hdf5::Attributes const& attributes);

                   Configuration       (Configuration const& other)=default;

                   ~Configuration      ()=default;

        Configuration& operator=       (Configuration const& other)=default;

        TimeDomainType
                   time_domain_type    () const;

        void       save                (hdf5::Attributes& attributes) const;

    private:

        TimeDomainType
                   _time_domain_type;

        void       load                (hdf5::Attributes const& attributes);

    };


                   Domain              (hdf5::Identifier const& location);

                   Domain              (hdf5::Group&& group);

                   Domain              (Domain const& other)=delete;

                   Domain              (Domain&& other)=default;

                   ~Domain             ()=default;

    Domain&        operator=           (Domain const& other)=delete;

    Domain&        operator=           (Domain&& other)=default;

    Configuration const&
                   configuration       () const;

    // TimeDomain&    time_domain         () const;

    // SpaceDomain&   space_domain        () const;

    // DomainConfiguration const&
    //                configuration       () const;

private:

    Configuration  _configuration;

    // std::unique_ptr<TimeDomain> _time_domain;

    // std::unique_ptr<SpaceDomain> _space_domain;

    // DomainConfiguration _configuration;

};


Domain             create_domain       (hdf5::Identifier const& location,
                                        Domain::Configuration const&
                                            configuration);

} // namespace lue
