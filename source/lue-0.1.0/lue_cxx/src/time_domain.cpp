#include "lue/time_domain.hpp"
#include "lue/time_unit_util.hpp"
#include "lue/compound/clock.hpp"
#include "lue/tag.hpp"


namespace lue {

TimeDomain::TimeDomain(
    hdf5::Group const& parent)

    : hdf5::Group{parent, time_domain_tag},
      _configuration{attributes()},
      _clock{time::Unit::year, 1000}

{
    {
        auto attribute = attributes().attribute(clock_tag);
        compound::Clock clock;
        auto const memory_datatype = compound::create_clock_memory_datatype();
        attribute.read(memory_datatype, clock);

        _clock = Clock(string_to_unit(clock.unit_name()), clock.nr_units());
    }
}


TimeDomain::TimeDomain(
    hdf5::Group&& group)

    : hdf5::Group{std::forward<hdf5::Group>(group)},
      _configuration{attributes()},
      _clock{time::Unit::year, 1000}

{
    {
        auto attribute = attributes().attribute(clock_tag);
        compound::Clock clock;
        auto const memory_datatype = compound::create_clock_memory_datatype();
        attribute.read(memory_datatype, clock);

        _clock = Clock(string_to_unit(clock.unit_name()), clock.nr_units());
    }
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
    Clock const& clock,
    TimeConfiguration const& configuration)
{
    auto group = hdf5::create_group(parent, time_domain_tag);

    {
        auto const file_datatype = compound::create_clock_file_datatype();
        hdf5::Dataspace dataspace{::H5S_SCALAR};
        auto attribute = group.attributes().add(
            clock_tag, file_datatype, dataspace);

        compound::Clock clock_(clock.unit(), clock.nr_units());

        auto const memory_datatype = compound::create_clock_memory_datatype();
        attribute.write(memory_datatype, clock_);
    }

    configuration.save(group.attributes());

    return TimeDomain{std::move(group)};
}


bool time_domain_exists(
    hdf5::Group const& parent)
{
    return parent.contains_group(time_domain_tag);
}

}  // namespace lue
