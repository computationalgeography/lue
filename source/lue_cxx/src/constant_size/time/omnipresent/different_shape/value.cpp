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

    : constant::different_shape::Collection{parent, name},
      omnipresent::Value()

{
}


Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : constant::different_shape::Collection{parent, name, memory_datatype},
      omnipresent::Value()

{
}


Value::Value(
    constant::different_shape::Collection&& collection)

    : constant::different_shape::Collection{
          std::forward<constant::different_shape::Collection>(collection)},
      omnipresent::Value()

{
}


hsize_t Value::nr_items() const
{
    return constant::different_shape::Collection::nr_items();
}


Value create_value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return Value{
        constant::different_shape::create_collection(
            parent, name, file_datatype, memory_datatype, rank)};
}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
