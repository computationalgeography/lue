#include "lue/cxx_api/constant_size/time_domain.h"


namespace lue {
namespace constant_size {

TimeDomain::TimeDomain(
    lue::TimeDomain& group)

    : _group{std::ref(group)}

{
}

}  // namespace constant_size
}  // namespace lue
