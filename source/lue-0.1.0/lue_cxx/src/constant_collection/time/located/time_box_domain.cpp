#include "lue/constant_collection/time/located/time_box_domain.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {

// TimeBoxDomain::TimeBoxDomain(
//     TimeDomain const& time_domain)
// 
//     : TimeDomain{time_domain.id()},
//       _items{
//           id(),
//           hdf5::Datatype{
//               hdf5::NativeDatatypeTraits<lue::time::DurationCount>::type_id()}}
// 
// {
// }


TimeBoxDomain::TimeBoxDomain(
    TimeDomain&& time_domain)

    : TimeDomain{std::forward<TimeDomain>(time_domain)},
      _items{
          *this,
          coordinates_tag,
          hdf5::Datatype{
              hdf5::NativeDatatypeTraits<lue::time::DurationCount>::type_id()}}

{
}


TimeBoxDomain::TimeBoxes const& TimeBoxDomain::items() const
{
    return _items;
}


TimeBoxDomain::TimeBoxes& TimeBoxDomain::items()
{
    return _items;
}


TimeBoxDomain::TimeBoxes& TimeBoxDomain::reserve(
    hsize_t const nr_items)
{
    _items.reserve(nr_items);

    return _items;
}


TimeBoxDomain create_time_box_domain(
    PropertySet& property_set,
    Clock const& clock)
{
    auto time_domain = located::create_time_domain(property_set.domain(),
        clock,
        TimeConfiguration(
            Occurrence::synchronous,
            TimeDomainItemType::box)
    );

    hdf5::Datatype memory_datatype(
        hdf5::NativeDatatypeTraits<lue::time::DurationCount>::type_id());
    hdf5::Datatype file_datatype(
        hdf5::StandardDatatypeTraits<lue::time::DurationCount>::type_id());

    // create_time_box(time_domain, file_datatype, memory_datatype);

    // A time box is defined by the start and end time points. Given a clock,
    // time points can be represented by durations since the clock's epoch.
    // Durations can be represented by an amount of ticks, which is just a
    // count.
    hdf5::Shape value_shape = { 2 };

    lue::same_shape::create_continuous_value(
        time_domain, coordinates_tag, file_datatype, memory_datatype,
        value_shape);

    return TimeBoxDomain{std::move(time_domain)}; // , memory_datatype);
}

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
