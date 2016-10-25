#include "lue/cxx_api/time/omnipresent/same_shape/property.h"
#include <cassert>


namespace lue {
namespace time {
namespace omnipresent {
namespace same_shape {

Property::Property(
    lue::Property& group,
    hid_t const type_id)

    : time::Property(group),
      _values{std::make_unique<same_shape::Item>(group.value().id(),
          "value", type_id)}

{
}


Item& Property::reserve_items(
    hsize_t const nr_items)
{
    _values->reserve_items(nr_items);

    return *_values;
}


Item& Property::values()
{
    return *_values;
}


void configure_property(
    lue::Property const& property,
    hid_t const file_type_id,
    hid_t const memory_type_id,
    Shape const& shape,
    Chunks const& chunks)
{
    same_shape::create_item(property.value().id(), "value",
        file_type_id, memory_type_id, shape, chunks);
}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace lue
