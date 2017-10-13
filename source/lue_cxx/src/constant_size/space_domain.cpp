#include "lue/constant_size/space_domain.hpp"


namespace lue {
namespace constant_size {

SpaceDomain::SpaceDomain(
    hdf5::Group const& group)

    : lue::SpaceDomain(group)

{
}


SpaceDomain::SpaceDomain(
    lue::SpaceDomain&& domain)

    : lue::SpaceDomain{std::forward<lue::SpaceDomain>(domain)}

{
}


// SpaceDomain::SpaceDomain(
//     SpaceDomain const& other)
// 
//     : lue::SpaceDomain(other)
// 
// {
// }


SpaceDomain create_space_domain(
    hdf5::Group const& group,
    SpaceDomain::Configuration const& configuration)
{
    auto domain = lue::create_space_domain(group, configuration);

    return std::move(domain);
}

}  // namespace constant_size
}  // namespace lue
