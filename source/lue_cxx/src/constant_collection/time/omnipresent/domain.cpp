#include "lue/constant_collection/time/omnipresent/domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

// Domain::Domain(
//     hdf5::Group const& parent)
// 
//     : constant_collection::Domain(parent)
// 
// {
// }


Domain::Domain(
    constant_collection::Domain&& domain)

    : constant_collection::Domain(std::forward<constant_collection::Domain>(domain))

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
//     auto domain = constant_collection::create_domain(location, configuration);
//     auto space = omnipresent::create_space_domain(domain);
// 
//     return Domain(std::move(domain));
// }

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
