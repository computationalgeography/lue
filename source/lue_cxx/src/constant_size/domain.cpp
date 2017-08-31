#include "lue/constant_size/domain.hpp"


namespace lue {
namespace constant_size {

Domain::Domain(
    hdf5::Identifier const& location)

    : lue::Domain(location)

{
}


Domain::Domain(
    lue::Domain&& domain)

    : lue::Domain{std::forward<lue::Domain>(domain)}

{
}


Domain create_domain(
    hdf5::Identifier const& location)
{
    auto domain = lue::create_domain(location);

    return Domain(std::move(domain));
}

} // namespace constant_size
} // namespace lue
