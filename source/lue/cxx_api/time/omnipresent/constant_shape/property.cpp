#include "lue/cxx_api/time/omnipresent/constant_shape/property.h"
#include <cassert>


namespace lue {
namespace time {
namespace omnipresent {
namespace constant_shape {

Property::Property(
    lue::Property& group,
    hid_t const type_id)

    : time::Property(group),
      _values{std::make_unique<constant_shape::Item>(id(), "values", type_id)}

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
    constant_shape::create_item(property.id(), "values", file_type_id,
        memory_type_id, shape, chunks);
}

}  // namespace constant_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace lue
