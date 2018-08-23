#include "lue/object/time/time_domain.hpp"
#include "lue/core/tag.hpp"


namespace lue {

void write(
    lue::Clock const& clock,
    hdf5::Attributes& attributes)
{
    Aspect<time::Unit>(clock.unit()).save(attributes);
    attributes.write(nr_time_units_tag, clock.nr_units());
}


Clock read(
    hdf5::Attributes const& attributes)
{
    auto const unit = Aspect<time::Unit>(attributes).value();
    auto const nr_units =
        attributes.read<time::TickPeriodCount>(nr_time_units_tag);

    return {unit, nr_units};
}


TimeDomain::TimeDomain(
    hdf5::Group& parent):

    hdf5::Group{parent, time_domain_tag},
    _configuration{attributes()},
    _clock{read(attributes())}

{
}


TimeDomain::TimeDomain(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _configuration{attributes()},
    _clock{read(attributes())}

{
}


TimeConfiguration const& TimeDomain::configuration() const
{
    return _configuration;
}


Clock const& TimeDomain::clock() const
{
    return _clock;
}


TimeDomain create_time_domain(
    hdf5::Group& parent,
    TimeConfiguration const& configuration,
    lue::Clock const& clock)
{
    auto group = hdf5::create_group(parent, time_domain_tag);

    switch(configuration.value<TimeDomainItemType>()) {
        case TimeDomainItemType::point: {
            create_time_point(group, clock);
            break;
        }
        case TimeDomainItemType::box: {
            create_time_box(group, clock);
            break;
        }
    }

    write(clock, group.attributes());

    configuration.save(group.attributes());

    return TimeDomain{std::move(group)};
}


void link_time_domain(
    hdf5::Group& parent,
    TimeDomain& domain)
{
    parent.create_soft_link(domain.id(), time_domain_tag);
}


bool has_linked_time_domain(
    hdf5::Group const& parent)
{
    return parent.contains_soft_link(time_domain_tag);
}


bool time_domain_exists(
    hdf5::Group const& parent)
{
    return parent.contains_group(time_domain_tag);
}

}  // namespace lue
