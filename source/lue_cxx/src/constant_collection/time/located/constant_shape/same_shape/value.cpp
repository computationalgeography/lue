#include "lue/constant_collection/time/located/constant_shape/same_shape/value.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {
namespace constant_shape {
namespace same_shape {

Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : constant_collection::constant_shape::same_shape::SynchronousVariable{
          parent, name, memory_datatype},
      constant_collection::Value()

{
}


Value::Value(
    constant_collection::constant_shape::same_shape::SynchronousVariable&& collection)

    : constant_collection::constant_shape::same_shape::SynchronousVariable{
          std::forward<constant_collection::constant_shape::same_shape::SynchronousVariable>(
              collection)},
      constant_collection::Value()

{
}


hsize_t Value::nr_items() const
{
    return constant_collection::constant_shape::same_shape::SynchronousVariable::nr_items();
}


Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype)
{
    return create_value(parent, name, file_datatype, memory_datatype,
        hdf5::Shape{});
}


Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    return Value{
        constant_collection::constant_shape::same_shape::create_synchronous_variable(
            parent, name, file_datatype, memory_datatype, value_shape)};
}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
