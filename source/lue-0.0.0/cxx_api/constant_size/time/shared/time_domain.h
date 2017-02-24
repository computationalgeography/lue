#pragma once
#include "lue/cxx_api/constant_size/time_domain.h"


namespace lue {
namespace constant_size {
namespace time {
namespace shared {

/*!
    @ingroup    lue_cxx_api_group
*/
class TimeDomain:
    public constant_size::TimeDomain
{

public:

                   TimeDomain          (lue::TimeDomain& group);

                   TimeDomain          (TimeDomain const& other)=delete;

                   TimeDomain          (TimeDomain&& other)=default;

                   ~TimeDomain         ()=default;

    TimeDomain&    operator=           (TimeDomain const& other)=delete;

    TimeDomain&    operator=           (TimeDomain&& other)=default;

private:

};

}  // namespace shared
}  // namespace time
}  // namespace constant_size
}  // namespace lue
