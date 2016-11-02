#include "lue/cxx_api/constant_size/time/space_domain.h"


namespace lue {
namespace constant_size {
namespace time {

SpaceDomain::SpaceDomain(
    lue::SpaceDomain& group)

    : _group{std::ref(group)}

{
}

} // namespace time
}  // namespace constant_size
} // namespace lue
