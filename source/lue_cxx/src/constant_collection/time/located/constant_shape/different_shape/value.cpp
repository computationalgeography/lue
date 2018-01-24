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

    : constant_collection::constant_shape::different_shape::Variable{parent, name},
      constant_collection::Value{}

{
}


Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : constant_collection::constant_shape::different_shape::Variable{
          parent, name, memory_datatype},
      constant_collection::Value{}

{
}


Value::Value(
    constant_collection::constant_shape::different_shape::Variable&& collection)

    : constant_collection::constant_shape::different_shape::Variable{
          std::forward<constant_collection::constant_shape::different_shape::Variable>(
              collection)},
      constant_collection::Value{}

{
}


hsize_t Value::nr_items() const
{
    return constant_collection::constant_shape::different_shape::Variable::nr_items();
}


Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return Value{
        constant_collection::constant_shape::different_shape::create_variable(
            parent, name, file_datatype, memory_datatype, rank)};
}

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
