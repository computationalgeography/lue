#include "lue/array/different_shape/constant_shape/value.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace different_shape {
namespace constant_shape {

/*!
    @brief      Open value @a name in @a parent
*/
Value::Value(
    hdf5::Group const& parent,
    std::string const& name)
:
    ValueGroup{parent, name},
    _nr_objects{attributes().read<Count>(nr_objects_tag)}

{
}


/*!
    @brief      Open value @a name in @a parent
*/
Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)
:
    ValueGroup{parent, name, memory_datatype},
    _nr_objects{attributes().read<Count>(nr_objects_tag)}

{
}


Value::Value(
    ValueGroup&& group)
:
    ValueGroup{std::forward<ValueGroup>(group)},
    _nr_objects{attributes().read<Count>(nr_objects_tag)}

{
}


Count Value::nr_objects() const
{
    return _nr_objects;
}


void Value::reserve(
    Count const nr_objects,
    ID const* ids,
    hdf5::Shape const* shapes,
    Count const* nr_locations_in_time)
{
    for(std::size_t o = 0; o < nr_objects; ++o) {
        reserve_(ids[o], shapes[o], nr_locations_in_time[o]);
    }

    _nr_objects += nr_objects;
    attributes().write<Count>(nr_objects_tag, _nr_objects);
}


same_shape::constant_shape::Value Value::reserve(
    ID const id,
    hdf5::Shape const& shape,
    Count const nr_locations_in_time)
{
    auto value = reserve_(id, shape, nr_locations_in_time);
    attributes().write<Count>(nr_objects_tag, ++_nr_objects);

    return value;
}


same_shape::constant_shape::Value Value::reserve_(
    ID const id,
    hdf5::Shape const& shape,
    Count const nr_locations_in_time)
{
    // Reserve space, but don't update the nr_objects_tag attribute
    std::string const name = std::to_string(id);
    auto value = same_shape::constant_shape::create_value(
        *this, name, file_datatype(), memory_datatype(), shape);
    value.reserve(nr_locations_in_time);

    return value;
}


same_shape::constant_shape::Value Value::operator[](
    ID const id)
{
    std::string const name = std::to_string(id);

    return same_shape::constant_shape::Value{*this, name, memory_datatype()};
}


/*!
    @brief      Create value @a name in @a parent
*/
Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    Rank const rank)
{
    return create_value(
        parent, name,
        file_datatype(memory_datatype), memory_datatype,
        rank);
}


/*!
    @brief      Create value @a name in @a parent
*/
Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    Rank const rank)
{
    auto group = create_value_group(
        parent, name, file_datatype, memory_datatype, rank);

    group.attributes().write<Count>(nr_objects_tag, 0);

    return Value{std::move(group)};
}

}  // namespace constant_shape
}  // namespace different_shape
}  // namespace lue
