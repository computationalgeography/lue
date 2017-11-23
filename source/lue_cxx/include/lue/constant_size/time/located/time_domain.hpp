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

                   TimeDomain          (hdf5::Identifier const& id);

                   TimeDomain          (lue::TimeDomain&& domain);

                   TimeDomain          (TimeDomain const& other)=delete;

                   TimeDomain          (TimeDomain&& other)=default;

                   ~TimeDomain         ()=default;

    TimeDomain&    operator=           (TimeDomain const& other)=delete;

    TimeDomain&    operator=           (TimeDomain&& other)=default;

};


TimeDomain        create_time_domain  (hdf5::Group const& group,
                                        TimeDomain::Configuration const&
                                            configuration);

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
