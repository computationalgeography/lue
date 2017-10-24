#pragma once
#include "lue/constant_size/time/located/time_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

class TimeDomain:
    public located::TimeDomain
{

public:

                   TimeDomain          (hdf5::Group const& group);

                   TimeDomain          (located::TimeDomain&& domain);

                   TimeDomain          (TimeDomain const& other)=delete;

                   TimeDomain          (TimeDomain&& other)=default;

                   ~TimeDomain         ()=default;

    TimeDomain&    operator=           (TimeDomain const& other)=delete;

    TimeDomain&    operator=           (TimeDomain&& other)=default;

};


TimeDomain        create_time_domain   (hdf5::Group const& group,
                                        TimeDomain::Configuration const&
                                            configuration);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
