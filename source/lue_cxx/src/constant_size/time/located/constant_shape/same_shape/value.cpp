#include "lue/constant_size/time/located/constant_shape/same_shape/value.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace constant_shape {
namespace same_shape {

Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : constant_size::constant_shape::same_shape::Variable{
          parent, name, memory_datatype},
      constant_size::Value()

{
}


Value::Value(
    constant_size::constant_shape::same_shape::Variable&& collection)

    : constant_size::constant_shape::same_shape::Variable{
          std::forward<constant_size::constant_shape::same_shape::Variable>(
              collection)},
      constant_size::Value()

{
}


hsize_t Value::nr_items() const
{
    return constant_size::constant_shape::same_shape::Variable::nr_items();
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
    return Value{constant_size::constant_shape::same_shape::create_variable(
        parent, name, file_datatype, memory_datatype, value_shape)};
}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
