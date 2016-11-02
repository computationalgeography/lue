#pragma once
#include "lue/cxx_api/constant_size/time/domain.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

/*!
    @ingroup    lue_cxx_api_group
*/
class Domain:
    public time::Domain
{

public:

                   Domain              (lue::Domain& group);

                   Domain              (Domain const& other)=delete;

                   Domain              (Domain&& other)=default;

                   ~Domain             ()=default;

    Domain&        operator=           (Domain const& other)=delete;

    Domain&        operator=           (Domain&& other)=default;

private:

};

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
