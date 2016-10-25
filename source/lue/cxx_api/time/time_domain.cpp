#include "lue/cxx_api/time/time_domain.h"


namespace lue {
namespace time {

TimeDomain::TimeDomain(
    lue::TimeDomain& group)

    : _group{std::ref(group)}

{
}

} // namespace time
} // namespace lue
