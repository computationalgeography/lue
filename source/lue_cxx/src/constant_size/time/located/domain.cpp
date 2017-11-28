#include "lue/constant_size/time/located/domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

Domain::Domain(
    hdf5::Group const& parent)

    : constant_size::Domain{parent}

{
}


Domain::Domain(
    constant_size::Domain&& domain)

    : constant_size::Domain(std::forward<constant_size::Domain>(domain))

{
}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
