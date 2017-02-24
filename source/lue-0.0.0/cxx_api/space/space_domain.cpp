#include "lue/cxx_api/space/space_domain.h"


namespace lue {
namespace space {

SpaceDomain::SpaceDomain(
    lue::SpaceDomain& group)

    : _group{std::ref(group)}

{
}

} // namespace space
} // namespace lue
