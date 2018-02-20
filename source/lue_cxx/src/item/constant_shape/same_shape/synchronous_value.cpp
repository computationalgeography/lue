#include "lue/item/constant_shape/same_shape/synchronous_value.hpp"


namespace lue {
namespace constant_shape {
namespace same_shape {

/*!
    @brief      Open collection @a name in @a parent
*/
SynchronousValue::SynchronousValue(
    hdf5::Group const& parent,
    std::string const& name)

    : Value{parent, name}

{
}


/*!
    @brief      Open collection @a name in @a parent
*/
SynchronousValue::SynchronousValue(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Value{parent, name, memory_datatype}

{
}


/*!
    @brief      Move in @a group
*/
SynchronousValue::SynchronousValue(
    hdf5::Group&& group,
    hdf5::Datatype const& memory_datatype)

    : Value{std::forward<hdf5::Group>(group), memory_datatype}

{
}


SynchronousValue create_synchronous_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    return create_synchronous_value(
        parent, name, hdf5::file_datatype(memory_datatype), memory_datatype,
        value_shape);
}


SynchronousValue create_synchronous_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    auto value = create_value(
        parent, name, file_datatype, memory_datatype, value_shape);

    return SynchronousValue{std::move(value), memory_datatype};
}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace lue
