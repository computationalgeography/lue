#include "lue/item/constant_shape/collection.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_shape {

/*!
    @brief      Create group @a name in @a parent

    The datatypes are of the individual values. The @a rank passed in
    defines the dimensionality of the item values.
*/
hdf5::Group create_group(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    int const rank)
{
    auto group = hdf5::create_group(parent, name);

    group.attributes().write<std::vector<unsigned char>>(
        datatype_tag, hdf5::encode_datatype(file_datatype));
    group.attributes().write<int>(rank_tag, rank);
    group.attributes().write<hsize_t>(nr_items_tag, 0);

    return group;
}


hdf5::Group create_group(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Shape const& value_shape)
{
    auto group = hdf5::create_group(parent, name);

    group.attributes().write<std::vector<unsigned char>>(
        datatype_tag, hdf5::encode_datatype(file_datatype));
    group.attributes().write(shape_tag, value_shape);
    group.attributes().write<hsize_t>(nr_items_tag, 0);

    return group;
}

}  // namespace constant_shape
}  // namespace lue
