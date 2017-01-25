#include "lue/cxx_api/constant_size/space_domain.h"


namespace lue {
namespace constant_size {

SpaceDomain::SpaceDomain(
    lue::SpaceDomain& group)

    : _group{std::ref(group)}

{
}

}  // namespace constant_size
}  // namespace lue
