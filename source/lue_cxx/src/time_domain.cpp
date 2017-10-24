#include "lue/time_domain.hpp"
#include "lue/compound/clock.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace {

detail::EnumStringBimap<time::Unit> const
        unit_map = {
    { time::Unit::second, "second" },
    { time::Unit::minute, "minute" },
    { time::Unit::hour, "hour" },
    { time::Unit::day, "day" },
    { time::Unit::week, "week" },
    { time::Unit::month, "month" },
    { time::Unit::year, "year" }
};


time::Unit parse_unit_name(
    std::string const& string)
{
    if(!unit_map.contains(string)) {
        throw std::runtime_error("Unknown time unit: " + string);
    }

    return unit_map.as_value(string);
}


detail::EnumStringBimap<TimeDomain::Configuration::Ownership> const
        ownership_map = {
    { TimeDomain::Configuration::Ownership::shared, "lue_shared" } // ,
    // { TimeDomain::Configuration::Ownership::unique, "lue_unique" }
};


std::string ownership_to_string(
    TimeDomain::Configuration::Ownership const ownership)
{
    return ownership_map.as_string(ownership);
}


TimeDomain::Configuration::Ownership parse_ownership(
    std::string const& string)
{
    if(!ownership_map.contains(string)) {
        throw std::runtime_error("Unknown time domain ownership: " + string);
    }

    return ownership_map.as_value(string);
}


detail::EnumStringBimap<TimeDomain::Configuration::ItemType> const
        item_type_map = {
    { TimeDomain::Configuration::ItemType::box, "lue_box" }
};


std::string item_type_to_string(
    TimeDomain::Configuration::ItemType const item_type)
{
    return item_type_map.as_string(item_type);
}


TimeDomain::Configuration::ItemType parse_item_type(
    std::string const& string)
{
    if(!item_type_map.contains(string)) {
        throw std::runtime_error("Unknown time domain item type: " + string);
    }

    return item_type_map.as_value(string);
}

}  // Anonymous namespace


TimeDomain::Configuration::Configuration(
    Clock const& clock,
    Ownership const ownership,
    ItemType const item_type)

    : _clock{clock},
      _ownership{ownership},
      _item_type{item_type}

{
}


TimeDomain::Configuration::Configuration(
    hdf5::Attributes const& attributes)

    : _clock(time::Unit::year, 1000),
      _ownership{TimeDomain::Configuration::Ownership::shared},
      _item_type{TimeDomain::Configuration::ItemType::box}

{
    load(attributes);
}


Clock const& TimeDomain::Configuration::clock() const
{
    return _clock;
}


TimeDomain::Configuration::Ownership
    TimeDomain::Configuration::ownership() const
{
    return _ownership;
}


TimeDomain::Configuration::ItemType
    TimeDomain::Configuration::item_type() const
{
    return _item_type;
}


void TimeDomain::Configuration::save(
    hdf5::Attributes& attributes) const
{
    {
        auto const file_datatype = compound::create_clock_file_datatype();
        hdf5::Dataspace dataspace{::H5S_SCALAR};
        auto attribute = attributes.add(clock_tag, file_datatype, dataspace);

        compound::Clock clock(_clock.unit(), _clock.nr_units());

        auto const memory_datatype = compound::create_clock_memory_datatype();
        attribute.write(memory_datatype, clock);
    }


    attributes.write<std::string>(
        time_domain_ownership_tag,
        ownership_to_string(_ownership)
    );

    attributes.write<std::string>(
        time_domain_item_type_tag,
        item_type_to_string(_item_type)
    );
}


void TimeDomain::Configuration::load(
    hdf5::Attributes const& attributes)
{
    {
        auto attribute = attributes.attribute(clock_tag);
        compound::Clock clock;
        auto const memory_datatype = compound::create_clock_memory_datatype();
        attribute.read(memory_datatype, clock);

        _clock = Clock(parse_unit_name(clock.unit_name()), clock.nr_units());
    }


    _ownership = parse_ownership(
        attributes.read<std::string>(time_domain_ownership_tag));
    _item_type = parse_item_type(
        attributes.read<std::string>(time_domain_item_type_tag));
}


TimeDomain::TimeDomain(
    hdf5::Group const& group)

    : hdf5::Group(group.id(), time_domain_tag),
      _configuration(attributes())

{
}


TimeDomain::TimeDomain(
    hdf5::Group&& group)

    : hdf5::Group{std::forward<hdf5::Group>(group)},
      _configuration(attributes())

{
}


TimeDomain::Configuration const& TimeDomain::configuration() const
{
    return _configuration;
}


TimeDomain create_time_domain(
    hdf5::Group const& group,
    TimeDomain::Configuration const& configuration)
{
    auto domain = hdf5::create_group(group, time_domain_tag);

    configuration.save(domain.attributes());

    return std::move(domain);
}


bool time_domain_exists(
    hdf5::Group const& group)
{
    return group.contains_group(time_domain_tag);
}

}  // namespace lue
