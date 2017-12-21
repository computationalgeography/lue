#include "lue/constant_size/time/located/shared/constant_shape/different_shape/value.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace different_shape {

Value::Value(
    hdf5::Group const& parent,
    std::string const& name)

    : variable::constant_shape::different_shape::Collection{parent, name},
      constant_size::Value{}

{
}


Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : variable::constant_shape::different_shape::Collection{
          parent, name, memory_datatype},
      constant_size::Value{}

{
}


Value::Value(
    variable::constant_shape::different_shape::Collection&& collection)

    : variable::constant_shape::different_shape::Collection{
          std::forward<variable::constant_shape::different_shape::Collection>(
              collection)},
      constant_size::Value{}

{
}


hsize_t Value::nr_items() const
{
    return variable::constant_shape::different_shape::Collection::nr_items();
}


Value create_value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return Value{
        variable::constant_shape::different_shape::create_collection(
            parent, name, file_datatype, memory_datatype, rank)};
}

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
