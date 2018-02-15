#include "lue/constant_collection/time/omnipresent/same_shape/value.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {
namespace same_shape {

Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : lue::constant_shape::same_shape::ContinuousValue{
        parent, name, memory_datatype},
      omnipresent::Value{}

{
}


Value::Value(
    lue::constant_shape::same_shape::ContinuousValue&& collection)

    : lue::constant_shape::same_shape::ContinuousValue{
          std::forward<lue::constant_shape::same_shape::ContinuousValue>(
              collection)},
      omnipresent::Value{}

{
}


hsize_t Value::nr_items() const
{
    return lue::constant_shape::same_shape::ContinuousValue::nr_items();
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
    return Value{lue::constant_shape::same_shape::create_continuous_value(
        parent, name, file_datatype, memory_datatype, value_shape)};
}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
