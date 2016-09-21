#include "lue/cxx_api/time/space_domain.h"


namespace lue {
namespace time {

SpaceDomain::SpaceDomain(
    lue::SpaceDomain& group)

    : _group{std::ref(group)}

{
}

} // namespace time
} // namespace lue
