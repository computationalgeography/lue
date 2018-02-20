#include "lue/item/same_shape/constant_shape/asynchronous_value.hpp"


namespace lue {
namespace same_shape {
namespace constant_shape {

/*!
    @brief      Open collection @a name in @a parent
*/
AsynchronousValue::AsynchronousValue(
    hdf5::Group const& parent,
    std::string const& name)

    : Value{parent, name}

{
}


/*!
    @brief      Open collection @a name in @a parent
*/
AsynchronousValue::AsynchronousValue(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Value{parent, name, memory_datatype}

{
}


/*!
    @brief      Move in @a group
*/
AsynchronousValue::AsynchronousValue(
    hdf5::Group&& group,
    hdf5::Datatype const& memory_datatype)

    : Value{std::forward<hdf5::Group>(group), memory_datatype}

{
}


AsynchronousValue create_asynchronous_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    return create_asynchronous_value(
        parent, name, hdf5::file_datatype(memory_datatype), memory_datatype,
        value_shape);
}


AsynchronousValue create_asynchronous_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    auto value = create_value(
        parent, name, file_datatype, memory_datatype, value_shape);

    return AsynchronousValue{std::move(value), memory_datatype};
}

}  // namespace constant_shape
}  // namespace same_shape
}  // namespace lue
