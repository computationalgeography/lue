#include "lue/constant_collection/time/located/constant_shape/different_shape/value.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {
namespace constant_shape {
namespace different_shape {

Value::Value(
    hdf5::Group const& parent,
    std::string const& name)

    : lue::constant_shape::different_shape::constant_collection::SynchronousValue{
        parent, name},
      constant_collection::Value{}

{
}


Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : lue::constant_shape::different_shape::constant_collection::SynchronousValue{
          parent, name, memory_datatype},
      constant_collection::Value{}

{
}


Value::Value(
    lue::constant_shape::different_shape::constant_collection::SynchronousValue&& collection)

    : lue::constant_shape::different_shape::constant_collection::SynchronousValue{
          std::forward<lue::constant_shape::different_shape::constant_collection::SynchronousValue>(
              collection)},
      constant_collection::Value{}

{
}


hsize_t Value::nr_items() const
{
    return lue::constant_shape::different_shape::constant_collection::SynchronousValue::nr_items();
}


Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return Value{
        lue::constant_shape::different_shape::constant_collection::create_synchronous_value(
            parent, name, file_datatype, memory_datatype, rank)};
}

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
