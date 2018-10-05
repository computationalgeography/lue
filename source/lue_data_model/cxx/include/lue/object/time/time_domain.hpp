#pragma once
#include "lue/info/time.hpp"
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

    Clock const&   clock               () const;

    template<
        typename T>
    T              value               ();

private:

    TimeConfiguration _configuration;

    Clock          _clock;

};


TimeDomain         create_time_domain  (hdf5::Group& parent,
                                        TimeConfiguration const&
                                            configuration,
                                        lue::Clock const& clock);

void               link_time_domain    (hdf5::Group& parent,
                                        TimeDomain& domain);

bool               has_linked_time_domain(
                                        hdf5::Group const& parent);

bool               time_domain_exists  (hdf5::Group const& parent);


template<
    typename T>
inline T TimeDomain::value()
{
    return T{*this};
}

}  // namespace lue
