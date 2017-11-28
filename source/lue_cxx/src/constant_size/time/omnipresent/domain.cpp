#include "lue/constant_size/time/omnipresent/domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

// Domain::Domain(
//     hdf5::Group const& parent)
// 
//     : constant_size::Domain(parent)
// 
// {
// }


Domain::Domain(
    constant_size::Domain&& domain)

    : constant_size::Domain(std::forward<constant_size::Domain>(domain))

{
}


// SpaceDomain const& Domain::space() const
// {
//     return _space;
// }
// 
// 
// SpaceDomain& Domain::space()
// {
//     return _space;
// }


// Domain create_domain(
//     hdf5::Identifier const& location,
//     Domain::Configuration const& configuration)
// {
//     auto domain = constant_size::create_domain(location, configuration);
//     auto space = omnipresent::create_space_domain(domain);
// 
//     return Domain(std::move(domain));
// }

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
