#include "lue/constant_size/time/located/shared/constant_shape/different_shape/value.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace different_shape {

Value::Value(
    hdf5::Identifier const& location,
    std::string const& name)

    : Group(location, name),
      constant_size::Value(),
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{hdf5::memory_datatype(_file_datatype)}

{
}


Value::Value(
    hdf5::Identifier const& location,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Group(location, name),
      constant_size::Value(),
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{memory_datatype}

{
}


Value::Value(
    hdf5::Group&& group,
    hdf5::Datatype const& memory_datatype)

    : Group(std::forward<hdf5::Group>(group)),
      constant_size::Value(),
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{memory_datatype}

{
}


hsize_t Value::nr_items() const
{
    return _nr_items;
}


int Value::rank() const
{
    return _rank;
}


hdf5::Datatype const& Value::file_datatype() const
{
    return _file_datatype;
}


hdf5::Datatype const& Value::memory_datatype() const
{
    return _memory_datatype;
}


void Value::reserve_value(
    hsize_t const idx,
    hsize_t const nr_time_domain_items,
    hsize_t const* shape)
{
    // Create dataset with this shape.
    std::string name = std::to_string(idx);

    hdf5::Shape dimension_sizes(shape, shape + _rank);
    dimension_sizes.insert(dimension_sizes.begin(), nr_time_domain_items);
    hdf5::Shape max_dimension_sizes(dimension_sizes);

    auto dataspace = hdf5::create_dataspace(
        dimension_sizes, max_dimension_sizes);

    // No chunking for now...
    hdf5::Dataset::CreationPropertyList creation_property_list;
    auto dataset = hdf5::create_dataset(
        id(), name, _file_datatype, dataspace, creation_property_list);
}


void Value::reserve(
    hsize_t const nr_time_domain_items,
    hsize_t const nr_items,
    hsize_t const* shapes)
{
    for(hsize_t i = 0; i < nr_items; ++i) {
        reserve_value(i, nr_time_domain_items, &(shapes[i * _rank]));
    }

    _nr_items = nr_items;

    attributes().write<hsize_t>(nr_items_tag, nr_items);
}


Array Value::operator[](
    size_t const idx) const
{
    return Array(id(), std::to_string(idx), _memory_datatype);
}


Value create_value(
    hdf5::Identifier const& location,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    auto group = hdf5::create_group(location, name);

    group.attributes().write<std::vector<unsigned char>>(datatype_tag,
        hdf5::encode_datatype(file_datatype));
    group.attributes().write<int>(rank_tag, rank);
    group.attributes().write<hsize_t>(nr_items_tag, 0);

    return Value(std::move(group), memory_datatype);
}

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
