#pragma once
#include "lue/cxx_api/space_domain.h"
#include <functional>


namespace lue {
namespace space {

/*!
    @ingroup    lue_cxx_api_group
*/
class SpaceDomain
{

public:

    virtual        ~SpaceDomain        ()=default;

protected:

                   SpaceDomain         (lue::SpaceDomain& group);

                   SpaceDomain         (SpaceDomain const& other)=delete;

                   SpaceDomain         (SpaceDomain&& other)=default;

    SpaceDomain&   operator=           (SpaceDomain const& other)=delete;

    SpaceDomain&   operator=           (SpaceDomain&& other)=default;

private:

    std::reference_wrapper<lue::SpaceDomain> _group;

};

} // namespace space
} // namespace lue
