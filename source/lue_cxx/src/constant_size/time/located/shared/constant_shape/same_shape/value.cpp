#include "lue/constant_size/time/located/shared/constant_shape/same_shape/value.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace same_shape {

Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : variable::constant_shape::same_shape::Collection{
          parent, name, memory_datatype},
      constant_size::Value()

{
}


Value::Value(
    variable::constant_shape::same_shape::Collection&& collection)

    : variable::constant_shape::same_shape::Collection{
          std::forward<variable::constant_shape::same_shape::Collection>(
              collection)},
      constant_size::Value()

{
}


hsize_t Value::nr_items() const
{
    return variable::constant_shape::same_shape::Collection::nr_items();
}


Value create_value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype)
{
    return create_value(parent, name, file_datatype, memory_datatype,
        hdf5::Shape{});
}


Value create_value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    return Value{variable::constant_shape::same_shape::create_collection(
        parent, name, file_datatype, memory_datatype, value_shape)};
}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
