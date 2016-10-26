#pragma once
#include "lue/cxx_api/constant_size/time/time_domain.h"


namespace lue {
namespace time {
namespace shared {

/*!
    @ingroup    lue_cxx_api_group
*/
class TimeDomain:
    public time::TimeDomain
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
}  // namespace lue
