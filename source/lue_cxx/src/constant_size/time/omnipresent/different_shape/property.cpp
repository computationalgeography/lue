#include "lue/constant_size/time/omnipresent/different_shape/property.hpp"
#include "lue/tag.hpp"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

hdf5::Datatype Property::file_datatype(
    hdf5::Identifier const& id)
{
    return hdf5::decode_datatype(
        hdf5::Group(id, value_tag).attributes()
            .read<std::vector<unsigned char>>(datatype_tag));
}


Property::Property(
    omnipresent::Property&& property,
    hdf5::Datatype const& memory_datatype)

    : omnipresent::Property(std::forward<omnipresent::Property>(property)),
      _values(id(), value_tag, memory_datatype)

{
}


Property::Property(
    lue::Property const& property,
    hdf5::Datatype const& memory_datatype)

    : omnipresent::Property(property),
      _values(id(), value_tag, memory_datatype)

{
}


// // // Property::Property(
// // //     hdf5::Identifier const& location,
// // //     std::string const& name)
// // // 
// // //     : time::omnipresent::Property(location, name),
// // //       _values{std::make_unique<different_shape::Item>(group.value().id(),
// // //           "value")}
// // // 
// // // {
// // //     if(!id().is_valid()) {
// // //         throw std::runtime_error("Property " + name + " cannot be opened");
// // //     }
// // // 
// // //     assert(value_exists(id()));
// // //     _value = std::make_unique<Value>(open_value(id()));
// // // }
// // 
// // 
// // Property::Property(
// //     lue::Property& group)
// // 
// //     : time::omnipresent::Property(group),
// //       _values{std::make_unique<different_shape::Item>(group.value().id(),
// //           "value")}
// // 
// // {
// // }
// // 
// // 
// // /*!
// //     @brief      .
// //     @param      type_id Type id of in-memory values
// //     @return     .
// //     @exception  .
// // */
// // Property::Property(
// //     lue::Property& group,
// //     hid_t const type_id)
// // 
// //     : time::omnipresent::Property(group),
// //       _values{std::make_unique<different_shape::Item>(group.value().id(),
// //           "value", type_id)}
// // 
// // {
// // }
// // 
// // 
// // Item& Property::reserve(
// //     hsize_t const nr_items)
// // {
// //     _values->reserve(nr_items);
// // 
// //     return *_values;
// // }
// // 
// // 
// // Item const& Property::values() const
// // {
// //     return *_values;
// // }
// // 
// // 
// // Item& Property::values()
// // {
// //     return *_values;
// // }
// 
// 
// // void configure_property(
// //     lue::Property const& property,
// //     hid_t const file_type_id,
// //     hid_t const memory_type_id,
// //     Shape const& shape,
// //     Chunks const& chunks)
// // {
// //     different_shape::create_item(property.value().id(), "value",
// //         file_type_id, memory_type_id, shape, chunks);
// // }


different_shape::Value const& Property::values() const
{
    return _values;
}


different_shape::Value& Property::values()
{
    return _values;
}


different_shape::Value& Property::reserve(
    hsize_t const nr_items,
    hsize_t const* shapes)
{
    _values.reserve(nr_items, shapes);

    return _values;
}


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return create_property(property_set, name,
        hdf5::file_datatype(memory_datatype), memory_datatype, rank);
}


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    Property::Configuration configuration(ShapePerItemType::different);
    auto property = omnipresent::create_property(property_set, name,
        configuration);
    auto value = create_value(property.id(), value_tag, file_datatype,
        memory_datatype, rank);

    assert(property.id().is_valid());

    return Property(std::move(property), memory_datatype);
}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
