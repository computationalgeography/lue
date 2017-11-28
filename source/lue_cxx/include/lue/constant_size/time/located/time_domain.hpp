#pragma once
#include "lue/time_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

class TimeDomain:
    public lue::TimeDomain
{

public:

                   TimeDomain          (hdf5::Group const& parent);

                   TimeDomain          (lue::TimeDomain&& domain);

                   TimeDomain          (TimeDomain const&)=delete;

                   TimeDomain          (TimeDomain&&)=default;

                   ~TimeDomain         ()=default;

    TimeDomain&    operator=           (TimeDomain const&)=delete;

    TimeDomain&    operator=           (TimeDomain&&)=default;

};


TimeDomain        create_time_domain   (hdf5::Group const& parent,
                                        TimeDomain::Configuration const&
                                            configuration);

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
