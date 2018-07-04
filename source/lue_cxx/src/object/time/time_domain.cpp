#include "lue/object/time/time_domain.hpp"
#include "lue/core/tag.hpp"


namespace lue {

TimeDomain::TimeDomain(
    hdf5::Group& parent):

    hdf5::Group{parent, time_domain_tag},
    _configuration{attributes()}

{
}


TimeDomain::TimeDomain(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _configuration{attributes()}

{
}


TimeConfiguration const& TimeDomain::configuration() const
{
    return _configuration;
}


TimeDomain create_time_domain(
    hdf5::Group& parent,
    TimeConfiguration const& configuration)
{
    auto group = hdf5::create_group(parent, time_domain_tag);

    configuration.save(group.attributes());

    return TimeDomain{std::move(group)};
}


bool time_domain_exists(
    hdf5::Group const& parent)
{
    return parent.contains_group(time_domain_tag);
}

}  // namespace lue
