#pragma once
#include "lue/cxx_api/space/space_domain.h"


namespace lue {
namespace space {
namespace stationary {

/*!
    @ingroup    lue_cxx_api_group
*/
class SpaceDomain:
    public space::SpaceDomain
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

}  // namespace stationary
}  // namespace space
}  // namespace lue
