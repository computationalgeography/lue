#include "lue/constant_size/time/located/shared/domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

Domain::Domain(
    hdf5::Group const& parent)

    : located::Domain(parent)

{
}


Domain::Domain(
    located::Domain&& domain)

    : located::Domain(std::forward<located::Domain>(domain))

{
}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue

