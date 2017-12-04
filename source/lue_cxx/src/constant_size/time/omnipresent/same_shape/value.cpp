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

    : constant_size::SameShape{parent, name, memory_datatype},
      omnipresent::Value{}

{
}


Value::Value(
    constant_size::SameShape&& item)

    : constant_size::SameShape{std::forward<constant_size::SameShape>(item)},
      omnipresent::Value{}

{
}


hsize_t Value::nr_items() const
{
    return constant_size::SameShape::nr_items();
}


Value create_value(
    hdf5::Group const& group,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype)
{
    return create_value(group, name, file_datatype, memory_datatype,
        hdf5::Shape{});
}


Value create_value(
    hdf5::Group const& group,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    return Value{constant_size::create_same_shape(
        group, name, file_datatype, memory_datatype, value_shape)};
}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
