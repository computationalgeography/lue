#pragma once
#include "lue/info/time.hpp"
#include "lue/core/clock.hpp"
#include "lue/core/configuration.hpp"
#include "lue/hdf5.hpp"


namespace lue {
namespace data_model {

using TimeConfiguration = Configuration<
    TimeDomainItemType
>;


class TimeDomain:
    public hdf5::Group
{

public:

    explicit       TimeDomain          (hdf5::Group const& parent);

    explicit       TimeDomain          (hdf5::Group&& group);

                   TimeDomain          (TimeDomain const&)=default;

                   TimeDomain          (TimeDomain&&)=default;

                   ~TimeDomain         () override =default;

    TimeDomain&    operator=           (TimeDomain const&)=default;

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
                                        data_model::Clock const& clock);

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

}  // namespace data_model
}  // namespace lue
