#include "lue/constant_size/time/omnipresent/different_shape/value.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

Value::Value(
    hdf5::Group const& parent,
    std::string const& name)

    : constant_shape::different_shape::Constant{parent, name},
      omnipresent::Value()

{
}


Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : constant_shape::different_shape::Constant{parent, name, memory_datatype},
      omnipresent::Value()

{
}


Value::Value(
    constant_shape::different_shape::Constant&& collection)

    : constant_shape::different_shape::Constant{
          std::forward<constant_shape::different_shape::Constant>(collection)},
      omnipresent::Value()

{
}


hsize_t Value::nr_items() const
{
    return constant_shape::different_shape::Constant::nr_items();
}


Value create_value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return Value{
        constant_shape::different_shape::create_constant(
            parent, name, file_datatype, memory_datatype, rank)};
}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
