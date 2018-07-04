#pragma once
#include "lue/core/configuration.hpp"
#include "lue/hdf5.hpp"


namespace lue {

using TimeConfiguration = Configuration<
    TimeDomainItemType
>;


class TimeDomain:
    public hdf5::Group
{

public:

    explicit       TimeDomain          (hdf5::Group& parent);

    explicit       TimeDomain          (hdf5::Group&& group);

                   TimeDomain          (TimeDomain const&)=delete;

                   TimeDomain          (TimeDomain&&)=default;

                   ~TimeDomain         ()=default;

    TimeDomain&    operator=           (TimeDomain const&)=delete;

    TimeDomain&    operator=           (TimeDomain&&)=default;

    TimeConfiguration const&
                   configuration       () const;

private:

    TimeConfiguration _configuration;

};


TimeDomain         create_time_domain  (hdf5::Group& parent,
                                        TimeConfiguration const&
                                            configuration);

bool               time_domain_exists  (hdf5::Group const& parent);

}  // namespace lue
