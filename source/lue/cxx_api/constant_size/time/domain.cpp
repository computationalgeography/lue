#include "lue/cxx_api/constant_size/time/domain.h"


namespace lue {
namespace constant_size {
namespace time {

Domain::Domain(
    lue::Domain& group)

    : _group{std::ref(group)}

{
}


TimeDomain& Domain::time_domain() const
{
    return _group.get().time_domain();
}


SpaceDomain& Domain::space_domain() const
{
    return _group.get().space_domain();
}


DomainConfiguration const& Domain::configuration() const
{
    return _group.get().configuration();
}

} // namespace time
}  // namespace constant_size
} // namespace lue
