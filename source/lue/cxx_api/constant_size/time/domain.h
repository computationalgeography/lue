#pragma once
#include "lue/cxx_api/domain.h"
#include <functional>


namespace lue {
namespace constant_size {
namespace time {

/*!
    @ingroup    lue_cxx_api_group
*/
class Domain
{

public:

    virtual        ~Domain             ()=default;

    TimeDomain&    time_domain         () const;

    SpaceDomain&   space_domain        () const;

    DomainConfiguration const&
                   configuration       () const;

protected:

                   Domain              (lue::Domain& group);

                   Domain              (Domain const& other)=delete;

                   Domain              (Domain&& other)=default;

    Domain&        operator=           (Domain const& other)=delete;

    Domain&        operator=           (Domain&& other)=default;

private:

    std::reference_wrapper<lue::Domain> _group;

};

} // namespace time
}  // namespace constant_size
} // namespace lue
