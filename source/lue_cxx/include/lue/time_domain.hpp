#pragma once
#include "lue/clock.hpp"
#include "lue/time/time_configuration.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {

class TimeDomain:
    public hdf5::Group
{

public:

                   TimeDomain          (hdf5::Group const& parent);

                   TimeDomain          (hdf5::Group&& group);

                   TimeDomain          (TimeDomain const&)=delete;

                   TimeDomain          (TimeDomain&&)=default;

                   ~TimeDomain         ()=default;

    TimeDomain&    operator=           (TimeDomain const&)=delete;

    TimeDomain&    operator=           (TimeDomain&&)=default;

    TimeConfiguration const&
                   configuration       () const;

    Clock const&   clock               () const;

private:

    TimeConfiguration  _configuration;

    Clock      _clock;

};


TimeDomain         create_time_domain  (hdf5::Group& parent,
                                        Clock const& clock,
                                        TimeConfiguration const& configuration);

bool               time_domain_exists  (hdf5::Group const& parent);

}  // namespace lue
