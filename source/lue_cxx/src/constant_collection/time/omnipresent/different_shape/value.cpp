#include "lue/constant_collection/time/omnipresent/different_shape/value.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {
namespace different_shape {

Value::Value(
    hdf5::Group const& parent,
    std::string const& name)

    : lue::constant_shape::different_shape::ContinuousValue{parent, name},
      omnipresent::Value()

{
}


Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : lue::constant_shape::different_shape::ContinuousValue{
        parent, name, memory_datatype},
      omnipresent::Value()

{
}


Value::Value(
    lue::constant_shape::different_shape::ContinuousValue&& collection)

    : lue::constant_shape::different_shape::ContinuousValue{
          std::forward<lue::constant_shape::different_shape::ContinuousValue>(
              collection)},
      omnipresent::Value()

{
}


hsize_t Value::nr_items() const
{
    return lue::constant_shape::different_shape::ContinuousValue::nr_items();
}


Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return Value{
        lue::constant_shape::different_shape::create_continuous_value(
            parent, name, file_datatype, memory_datatype, rank)};
}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
