#include "lue/constant_size/time/omnipresent/same_shape/value.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {

Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : constant_shape::same_shape::Constant{parent, name, memory_datatype},
      omnipresent::Value{}

{
}


Value::Value(
    constant_shape::same_shape::Constant&& collection)

    : constant_shape::same_shape::Constant{
          std::forward<constant_shape::same_shape::Constant>(collection)},
      omnipresent::Value{}

{
}


hsize_t Value::nr_items() const
{
    return constant_shape::same_shape::Constant::nr_items();
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
    return Value{constant_shape::same_shape::create_constant(
        parent, name, file_datatype, memory_datatype, value_shape)};
}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
