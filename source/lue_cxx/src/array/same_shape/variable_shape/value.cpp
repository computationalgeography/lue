#include "lue/array/same_shape/variable_shape/value.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace same_shape {
namespace variable_shape {

/*!
    @brief      Open value @a name in @a parent
*/
Value::Value(
    hdf5::Group const& parent,
    std::string const& name)
:
    hdf5::Group{parent, name},
    _file_datatype{hdf5::decode_datatype(
        attributes().read<std::vector<unsigned char>>(datatype_tag))},
    _memory_datatype{hdf5::memory_datatype(_file_datatype)},
    _rank{attributes().read<Rank>(rank_tag)},
    _nr_locations_in_time{attributes().read<Count>(nr_locations_in_time_tag)}

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
    hdf5::Group{parent, name},
    _file_datatype{hdf5::decode_datatype(
        attributes().read<std::vector<unsigned char>>(datatype_tag))},
    _memory_datatype{memory_datatype},
    _rank{attributes().read<Rank>(rank_tag)},
    _nr_locations_in_time{attributes().read<Count>(nr_locations_in_time_tag)}

{
}


Value::Value(
    Group&& group,
    hdf5::Datatype const& memory_datatype)
:
    Group{std::forward<hdf5::Group>(group)},
    _file_datatype{hdf5::decode_datatype(
        attributes().read<std::vector<unsigned char>>(datatype_tag))},
    _memory_datatype{memory_datatype},
    _rank{attributes().read<Rank>(rank_tag)},
    _nr_locations_in_time{attributes().read<Count>(nr_locations_in_time_tag)}

{
}


hdf5::Datatype const& Value::file_datatype() const
{
    return _file_datatype;
}


hdf5::Datatype const& Value::memory_datatype() const
{
    return _memory_datatype;
}


Rank Value::rank() const
{
    return _rank;
}


Count Value::nr_locations_in_time() const
{
    return _nr_locations_in_time;
}


/*!
    @brief      Reserve space for @a nr_arrays object arrays
    @param      idx Index of location in time for which space must
                be reserved
*/
same_shape::Value Value::reserve(
    Index idx,
    Count nr_arrays,
    hdf5::Shape const& array_shape)
{
    std::string const name = std::to_string(idx);
    auto value = same_shape::create_value(
        *this, name, _file_datatype, _memory_datatype, array_shape);
    value.reserve(nr_arrays);
    attributes().write<Count>(
        nr_locations_in_time_tag, ++_nr_locations_in_time);

    return value;
}


same_shape::Value Value::operator[](
    Index idx)
{
    std::string const name = std::to_string(idx);

    return same_shape::Value{*this, name};
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
    auto group = hdf5::create_group(parent, name);

    group.attributes().write<std::vector<unsigned char>>(
        datatype_tag, hdf5::encode_datatype(file_datatype));
    group.attributes().write<Rank>(rank_tag, rank);
    group.attributes().write<Count>(nr_locations_in_time_tag, 0);

    return Value{std::move(group), memory_datatype};
}

}  // namespace variable_shape
}  // namespace same_shape
}  // namespace lue
