#include "lue/constant_collection/time/located/domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {

Domain::Domain(
    hdf5::Group const& parent)

    : constant_collection::Domain{parent}

{
}


Domain::Domain(
    constant_collection::Domain&& domain)

    : constant_collection::Domain(std::forward<constant_collection::Domain>(domain))

{
}

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
