#include "lue/constant_collection/domain.hpp"


namespace lue {
namespace constant_collection {

Domain::Domain(
    hdf5::Group const& parent)

    : lue::Domain(parent)

{
}


Domain::Domain(
    lue::Domain&& domain)

    : lue::Domain{std::forward<lue::Domain>(domain)}

{
}


// Domain create_domain(
//     hdf5::Identifier const& location,
//     Domain::Configuration const& configuration)
// {
//     auto domain = lue::create_domain(location, configuration);
// 
//     return Domain(std::move(domain));
// }

} // namespace constant_collection
} // namespace lue
