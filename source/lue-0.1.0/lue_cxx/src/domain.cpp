#include "lue/domain.hpp"
#include "lue/tag.hpp"


namespace lue {

Domain::Domain(
    hdf5::Group const& parent)

    : hdf5::Group(parent, domain_tag)

{
}


Domain::Domain(
    hdf5::Group&& group)

    : hdf5::Group{std::forward<hdf5::Group>(group)}

{
}


Domain create_domain(
    hdf5::Group& parent)
{
    auto group = hdf5::create_group(parent, domain_tag);

    return Domain{std::move(group)};
}

} // namespace lue
