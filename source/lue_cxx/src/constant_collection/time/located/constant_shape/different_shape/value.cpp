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

    : constant_collection::constant_shape::different_shape::SynchronousVariable{parent, name},
      constant_collection::Value{}

{
}


Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : constant_collection::constant_shape::different_shape::SynchronousVariable{
          parent, name, memory_datatype},
      constant_collection::Value{}

{
}


Value::Value(
    constant_collection::constant_shape::different_shape::SynchronousVariable&& collection)

    : constant_collection::constant_shape::different_shape::SynchronousVariable{
          std::forward<constant_collection::constant_shape::different_shape::SynchronousVariable>(
              collection)},
      constant_collection::Value{}

{
}


hsize_t Value::nr_items() const
{
    return constant_collection::constant_shape::different_shape::SynchronousVariable::nr_items();
}


Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return Value{
        constant_collection::constant_shape::different_shape::create_synchronous_variable(
            parent, name, file_datatype, memory_datatype, rank)};
}

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
