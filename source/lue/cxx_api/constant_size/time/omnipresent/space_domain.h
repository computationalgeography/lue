#pragma once
#include "lue/cxx_api/constant_size/time/space_domain.h"


namespace lue {
namespace time {
namespace omnipresent {

/*!
    @ingroup    lue_cxx_api_group
*/
class SpaceDomain:
    public time::SpaceDomain
{

public:

                   SpaceDomain         (lue::SpaceDomain& group);

                   SpaceDomain         (SpaceDomain const& other)=delete;

                   SpaceDomain         (SpaceDomain&& other)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const& other)=delete;

    SpaceDomain&   operator=           (SpaceDomain&& other)=default;

private:

};

}  // namespace omnipresent
}  // namespace time
}  // namespace lue
