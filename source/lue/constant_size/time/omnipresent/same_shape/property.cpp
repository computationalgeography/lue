#include "lue/constant_size/time/omnipresent/same_shape/property.h"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {


Property::Property(
    omnipresent::Property&& property,
    hdf5::Datatype const& memory_datatype)

    : omnipresent::Property(std::forward<omnipresent::Property>(property)),
      _values(id(), "lue_values", memory_datatype)

{
}


// // Property::Property(
// //     hdf5::Identifier const& location,
// //     std::string const& name)
// // 
// //     : time::omnipresent::Property(location, name),
// //       _values{std::make_unique<same_shape::Item>(group.value().id(),
// //           "value")}
// // 
// // {
// //     if(!id().is_valid()) {
// //         throw std::runtime_error("Property " + name + " cannot be opened");
// //     }
// // 
// //     assert(value_exists(id()));
// //     _value = std::make_unique<Value>(open_value(id()));
// // }
// 
// 
// Property::Property(
//     lue::Property& group)
// 
//     : time::omnipresent::Property(group),
//       _values{std::make_unique<same_shape::Item>(group.value().id(),
//           "value")}
// 
// {
// }
// 
// 
// /*!
//     @brief      .
//     @param      type_id Type id of in-memory values
//     @return     .
//     @exception  .
// */
// Property::Property(
//     lue::Property& group,
//     hid_t const type_id)
// 
//     : time::omnipresent::Property(group),
//       _values{std::make_unique<same_shape::Item>(group.value().id(),
//           "value", type_id)}
// 
// {
// }
// 
// 
// Item& Property::reserve_items(
//     hsize_t const nr_items)
// {
//     _values->reserve_items(nr_items);
// 
//     return *_values;
// }
// 
// 
// Item const& Property::values() const
// {
//     return *_values;
// }
// 
// 
// Item& Property::values()
// {
//     return *_values;
// }


// void configure_property(
//     lue::Property const& property,
//     hid_t const file_type_id,
//     hid_t const memory_type_id,
//     Shape const& shape,
//     Chunks const& chunks)
// {
//     same_shape::create_item(property.value().id(), "value",
//         file_type_id, memory_type_id, shape, chunks);
// }


same_shape::Value const& Property::values() const
{
    return _values;
}


same_shape::Value& Property::values()
{
    return _values;
}


same_shape::Value& Property::reserve_values(
    hsize_t const nr_items)
{
    _values.reserve_values(nr_items);

    return _values;
}


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const file_datatype,
    hdf5::Datatype const memory_datatype,
    hdf5::Shape const& value_shape,
    hdf5::Shape const& value_chunk)
{
    Property::Configuration configuration(ShapePerItemType::same);
    auto property = omnipresent::create_property(property_set, name,
        configuration);
    auto value = create_value(property.id(), "lue_value", file_datatype,
        memory_datatype, value_shape, value_chunk);

    return Property(std::move(property), memory_datatype);
}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
