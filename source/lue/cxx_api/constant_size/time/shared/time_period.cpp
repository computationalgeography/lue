#include "lue/cxx_api/constant_size/time/shared/time_period.h"
#include <cassert>


namespace lue {
namespace time {
namespace shared {

TimePeriod::TimePeriod(
    hdf5::Identifier const& location)

    : omnipresent::same_shape::Item(location, "coordinates")

{
}


TimePeriod::TimePeriod(
    hdf5::Identifier const& location,
    hid_t const type_id)

    : omnipresent::same_shape::Item(location, "coordinates", type_id)

{
}


TimePeriod::TimePeriod(
    omnipresent::same_shape::Item&& coordinates)

    : omnipresent::same_shape::Item(
        std::forward<omnipresent::same_shape::Item>(coordinates))

{
}


/*!
    @ingroup    lue_cxx_api_group
*/
TimePeriod create_time_period(
    hdf5::Identifier const& location,
    hid_t const file_type_id,
    hid_t const memory_type_id)
{
    std::string const name = "coordinates";
    // (start year, start month, nr_months)
    Shape shape = { 3 };
    Chunks chunks = { shape[0] };

    auto item = omnipresent::same_shape::create_item(location, name,
        file_type_id, memory_type_id, shape, chunks);

    return TimePeriod(std::move(item));
}

}  // namespace shared
}  // namespace time
}  // namespace lue
