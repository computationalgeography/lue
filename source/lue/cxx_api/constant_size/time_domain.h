#pragma once
#include "lue/cxx_api/time_domain.h"
#include <functional>


namespace lue {
namespace constant_size {

/*!
    @ingroup    lue_cxx_api_group
*/
class TimeDomain
{

public:

    virtual        ~TimeDomain         ()=default;

protected:

                   TimeDomain          (lue::TimeDomain& group);

                   TimeDomain          (TimeDomain const& other)=delete;

                   TimeDomain          (TimeDomain&& other)=default;

    TimeDomain&    operator=           (TimeDomain const& other)=delete;

    TimeDomain&    operator=           (TimeDomain&& other)=default;

private:

    std::reference_wrapper<lue::TimeDomain> _group;

};

}  // namespace constant_size
}  // namespace lue
