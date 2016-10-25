#include "lue/cxx_api/time/omnipresent/different_shape/property.h"
#include <cassert>


namespace lue {
namespace time {
namespace omnipresent {
namespace different_shape {

Property::Property(
    lue::Property& group)

    : time::Property(group),
      _values(group.value().id(), "value")

{
}


Item& Property::reserve_items(
    hsize_t const nr_items,
    extent_t const* shapes)
{
    _values.reserve_items(nr_items, shapes);

    return _values;
}


Item& Property::values()
{
    return _values;
}


void configure_property(
    lue::Property const& property,
    hid_t const file_type_id,
    size_t const rank)
{
    assert(property.id().is_valid());

    different_shape::create_item(property.value().id(), "value",
        file_type_id, rank);
}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace lue
