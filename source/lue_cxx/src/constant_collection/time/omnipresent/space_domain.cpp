#include "lue/constant_collection/time/omnipresent/space_domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

SpaceDomain::SpaceDomain(
    hdf5::Group const& parent)

    : constant_collection::SpaceDomain(parent)

{
}


SpaceDomain::SpaceDomain(
    constant_collection::SpaceDomain&& domain)

    : constant_collection::SpaceDomain{
          std::forward<constant_collection::SpaceDomain>(domain)}

{
}


// SpaceDomain::SpaceDomain(
//     SpaceDomain const& other)
// 
//     : constant_collection::SpaceDomain(other)
// 
// {
// }


SpaceDomain create_space_domain(
    hdf5::Group const& parent,
    SpaceConfiguration const& configuration)
{
    auto domain = constant_collection::create_space_domain(
        parent, configuration);

    return std::move(domain);
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
