#include "lue/constant_size/time/located/shared/time_box_domain.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

// hdf5::Datatype TimeBoxDomain::file_datatype(
//     hdf5::Identifier const& id)
// {
//     return hdf5::Dataset(id, coordinates_tag).datatype();
// }


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
          hdf5::Datatype{
              hdf5::NativeDatatypeTraits<lue::time::DurationCount>::type_id()}}

{
}


TimeBox const& TimeBoxDomain::items() const
{
    return _items;
}


TimeBox& TimeBoxDomain::items()
{
    return _items;
}


TimeBox& TimeBoxDomain::reserve(
    hsize_t const nr_items)
{
    _items.reserve(nr_items);

    return _items;
}


TimeBoxDomain create_time_box_domain(
    PropertySet& property_set,
    Clock const& clock)
{
    auto& domain = property_set.domain();

    auto time_domain = located::shared::create_time_domain(domain,
        TimeDomain::Configuration(
            clock,
            TimeDomain::Configuration::Ownership::shared,
            TimeDomain::Configuration::ItemType::box)
    );

    hdf5::Datatype memory_datatype(
        hdf5::NativeDatatypeTraits<lue::time::DurationCount>::type_id());
    hdf5::Datatype file_datatype(
        hdf5::StandardDatatypeTraits<lue::time::DurationCount>::type_id());

    create_time_box(time_domain, file_datatype, memory_datatype);

    return TimeBoxDomain{std::move(time_domain)}; // , memory_datatype);
}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
